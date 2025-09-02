#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "host/ble_hs.h"
#include "esp_central.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "services/gap/ble_svc_gap.h"
#include "nimble/nimble_port_freertos.h"

#include "setting.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define UART_PORT UART_NUM_0
#define BUF_SIZE (2 * SOC_UART_FIFO_LEN)

#define TAG "CLIENT"
#define DEVICE_NAME "Alex client"
#define SERVER_ADDRESS "c0:dc:0f:fe:ee:01"
#define THIS_ADDRESS "c0:12:34:56:78:9a"

#define GATT_SVC_UUID 0xABF0 /* 16 Bit Service UUID */
#define GATT_CHR_UUID 0xABF1 /* 16 Bit Service Characteristic UUID */

static void ble_client_scan(void);
static int ble_client_gap_event(struct ble_gap_event *event, void *arg);

// static variabel sätts automatiskt till 0
static ble_addr_t connected_addr;
static uint8_t own_addr_type;

// Aktiverar "notifieringar" från serven (om stöd finns)
static void ble_client_write_subscribe(const struct peer *peer)
{
    const struct peer_dsc *dsc = peer_dsc_find_uuid(peer, BLE_UUID16_DECLARE(GATT_SVC_UUID),
                                                    BLE_UUID16_DECLARE(GATT_CHR_UUID),
                                                    BLE_UUID16_DECLARE(BLE_GATT_DSC_CLT_CFG_UUID16));
    if (dsc != NULL)
    {
        uint8_t value[2] = {1, 0};
        int status = ble_gattc_write_flat(peer->conn_handle, dsc->dsc.handle, value, sizeof(value), NULL, NULL);
        if (status != 0)
        {
            ESP_LOGE(TAG, "Error: Failed to subscribe to characteristic; err = %d\n", status);
            ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM); /* Terminate the connection. */
        }
    }
    else
    {
        ESP_LOGE(TAG, "Error: Peer lacks a CCCD for the Unread Alert Status characteristic\n");
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM); /* Terminate the connection. */
    }
}

// Called when service discovery of the specified peer has completed.
// Efter anslutningen: söker efter tjänster/karaktärer
static void ble_client_on_disc_complete(const struct peer *peer, int status, void *)
{
    if (status == 0)
    {
        /* Service discovery has completed successfully.  Now we have a complete
         * list of services, characteristics, and descriptors that the peer supports. */
        ESP_LOGI(TAG, "Service discovery complete; status=%d conn_handle=%d\n", status, peer->conn_handle);

        // ble_client_set_handle(peer);
        ble_client_write_subscribe(peer);
    }
    else
    {
        /* Service discovery failed.  Terminate the connection. */
        ESP_LOGE(TAG, "Error: Service discovery failed; status=%d conn_handle=%d\n", status, peer->conn_handle);
        ble_gap_terminate(peer->conn_handle, BLE_ERR_REM_USER_CONN_TERM);
    }

    // Nu ska uppkopplingen vara klar. Skulle kunna skriva något log meddelande om det här.
}

// Initiates the GAP general discovery procedure.
// Börjar leta efter BLE-enheter
static void ble_client_scan(void)
{
    struct ble_gap_disc_params disc_params = {0};

    disc_params.passive = 1;           /* Perform a passive scan. */
    disc_params.filter_duplicates = 1; /* Avoid processing repeated advertisements from the same device. */

    int status = ble_gap_disc(own_addr_type, BLE_HS_FOREVER, &disc_params, ble_client_gap_event, NULL);
    if (status != 0)
    {
        ESP_LOGE(TAG, "Error initiating GAP discovery procedure; rc=%d\n", status);
    }
}

// Försöker ansluta till en specifik server
static void ble_client_connect(const struct ble_gap_disc_desc *disc)
{
    /* Scanning must be stopped before a connection can be initiated. */
    int status = ble_gap_disc_cancel();

    if (status == 0)
    {
        uint8_t own_addr_type;

        status = ble_hs_id_infer_auto(0, &own_addr_type); /* Figure out address to use for connect */

        if (status == 0)
        {
            /* Try to connect the advertiser.  Allow 30 seconds (30000 ms) for timeout. It can be BLE_HS_FOREVER */
            status = ble_gap_connect(own_addr_type, &disc->addr, 50000, NULL, ble_client_gap_event, NULL);
            if (status != 0)
            {
                ESP_LOGE(TAG, "Error: Failed to connect to device; addr_type=%d addr=%s; status=%d\n",
                         disc->addr.type, addr_str(disc->addr.val), status);
            }
        }
        else
        {
            ESP_LOGE(TAG, "Error determining address type; status=%d\n", status);
        }
    }
    else
    {
        ESP_LOGE(TAG, "Failed to cancel scan; status=%d\n", status);
    }
}

// hanterar alla bluetooth händelser
static int ble_client_gap_event(struct ble_gap_event *event, void *)
{
    int status = 0;
    struct ble_gap_conn_desc desc;
    struct ble_hs_adv_fields fields;

    switch (event->type)
    {
    case BLE_GAP_EVENT_DISC:
        status = ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
        if (status == 0)
        {
            /* An advertisement report was received during GAP discovery. */
            print_adv_fields(&fields);

            bool connected = false;
            /* Check if device is already connected or not */
            if (0 == memcmp(&connected_addr.val, event->disc.addr.val, sizeof(event->disc.addr.val)))
            {
                ESP_LOGI(TAG, "Device already connected");
                connected = true;
                break;
            }

            if (!connected)
            {
                /* The device has to be advertising connectability. */
                if ((event->disc.event_type == BLE_HCI_ADV_RPT_EVTYPE_ADV_IND) || (event->disc.event_type == BLE_HCI_ADV_RPT_EVTYPE_DIR_IND))
                {
                    uint8_t server_addr[sizeof(event->disc.addr.val)];
                    sscanf(SERVER_ADDRESS, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                           &server_addr[5], &server_addr[4], &server_addr[3],
                           &server_addr[2], &server_addr[1], &server_addr[0]);

                    if (0 == memcmp(event->disc.addr.val, server_addr, sizeof(server_addr)))
                    {
                        /* The device has to advertise support for the service (0xABF0). */
                        for (int i = 0; i < fields.num_uuids16; i++)
                        {
                            if (ble_uuid_u16(&fields.uuids16[i].u) == GATT_SVC_UUID)
                            {
                                /* Try to connect to the advertiser. */
                                ble_client_connect(&event->disc);
                                break;
                            }
                        }
                    }
                }
            }
        }
        break;

    case BLE_GAP_EVENT_CONNECT: /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0)
        {
            /* Connection successfully established. */
            ESP_LOGI(TAG, "Connection established ");
            assert(0 == ble_gap_conn_find(event->connect.conn_handle, &desc));
            print_conn_desc(&desc);

            memcpy(&connected_addr.val, desc.peer_id_addr.val, sizeof(desc.peer_id_addr.val));

            /* Remember peer. */
            status = peer_add(event->connect.conn_handle);
            if (status == 0)
            {
                /* Perform service discovery. */
                status = peer_disc_all(event->connect.conn_handle, ble_client_on_disc_complete, NULL);
                if (status != 0)
                {
                    ESP_LOGE(TAG, "Failed to discover services; status=%d\n", status);
                }
            }
            else
            {
                ESP_LOGE(TAG, "Failed to add peer; status=%d\n", status);
            }
        }
        else
        {
            /* Connection attempt failed; resume scanning. */
            ESP_LOGE(TAG, "Error: Connection failed; status=%d\n", event->connect.status);
            ble_client_scan();
        }

        break;

    case BLE_GAP_EVENT_DISCONNECT:
        /* Connection terminated. */
        ESP_LOGI(TAG, "disconnect; reason=%d ", event->disconnect.reason);
        print_conn_desc(&event->disconnect.conn);

        /* Forget about peer. */
        memset(&connected_addr.val, 0, sizeof(connected_addr.val));
        peer_delete(event->disconnect.conn.conn_handle);

        /* Resume scanning. */
        ble_client_scan();
        break;

    case BLE_GAP_EVENT_DISC_COMPLETE:
        ESP_LOGI(TAG, "discovery complete; reason=%d\n", event->disc_complete.reason);
        break;

    case BLE_GAP_EVENT_NOTIFY_RX:
    {
        char buffer[OS_MBUF_PKTLEN(event->notify_rx.om)];
        memset(buffer, 0, sizeof(buffer));

        // Attribute data is contained in event->notify_rx.om.
        assert(0 == os_mbuf_copydata(event->notify_rx.om, 0, sizeof(buffer), buffer));

        // printf("\n    Delivered: %.*s\n", 3, buffer);

        uart_write_bytes(UART_PORT, buffer, sizeof(buffer));
        fflush(stdout);
    }
    break;

    case BLE_GAP_EVENT_MTU:
        /* Maximum Transmission Unit defines the maximum size of a single ATT (Attribute Protocol) payload,
         i.e., how much data can be sent in a single BLE GATT read/write/notify/indication operation. */
        ESP_LOGI(TAG, "MTU update event; conn_handle=%d cid=%d mtu=%d\n",
                 event->mtu.conn_handle, event->mtu.channel_id, event->mtu.value);
        break;

    default:
        break;
    }

    return status;
}

static void ble_client_on_reset(int reason)
{
    ESP_LOGE(TAG, "Resetting state; reason=%d\n", reason);
}

static void ble_client_on_sync(void)
{
    uint8_t this_addr_val[6] = {0};
    sscanf(THIS_ADDRESS, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &this_addr_val[5], &this_addr_val[4], &this_addr_val[3],
           &this_addr_val[2], &this_addr_val[1], &this_addr_val[0]);

    this_addr_val[5] |= 0xC0;
    own_addr_type = BLE_OWN_ADDR_RANDOM;

    assert(0 == ble_hs_id_set_rnd(this_addr_val));

    printf("BLE Device Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           this_addr_val[5], this_addr_val[4], this_addr_val[3], this_addr_val[2], this_addr_val[1], this_addr_val[0]);

    /* Begin scanning for a peripheral to connect to. */
    ble_client_scan();
}

// initsierar och startar NimBLE-stack
void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate = BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};

    // Installera UART-drivrutin
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    esp_err_t status = nvs_flash_init();
    if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        status = nvs_flash_init();
    }
    ESP_ERROR_CHECK(status);

    ESP_ERROR_CHECK(nimble_port_init());

    /* Configure the host. */
    ble_hs_cfg.reset_cb = ble_client_on_reset;
    ble_hs_cfg.sync_cb = ble_client_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Initialize data structures to track connected peers. */
    assert(0 == peer_init(CONFIG_BT_NIMBLE_MAX_CONNECTIONS, 64, 64, 64));

    /* Set the default device name. */
    assert(0 == ble_svc_gap_device_name_set(DEVICE_NAME));

    ESP_LOGI(TAG, "BLE Host Task Started");
    nimble_port_run(); /* This function will return only when nimble_port_stop() is executed */
    nimble_port_freertos_deinit();
}
