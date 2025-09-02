#include <QDebug>
#include <QSerialPort>
#include "uartservice.h"

void UARTService::run()
{
    QSerialPort serial;
    uint8_t temp[sizeof(buffer)]{0};

    serial.setPortName(UART_CPORT);
    serial.setBaudRate(BAUDRATE);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    while (!end)
    {
        if (serial.open(QIODevice::ReadOnly))
        {
            while (!end && serial.isReadable())
            {
                (void)serial.clear();

                if (serial.waitForReadyRead(settings::DRAW_INTERVAL * 13))
                {
                    if (sizeof(temp) == serial.read(reinterpret_cast<char *>(temp), sizeof(temp)))
                    {
                        status = true;
                        std::scoped_lock<std::mutex> lock{buffer_mutex};
                        std::memcpy(buffer, temp, sizeof(temp));
                    }
                    else
                    {
                        status = false;
                        break;
                    }
                }
                else
                {
                    status = false;
                    break;
                }
            }
        }
        else
        {
            status = false;
        }

        if (serial.isOpen())
        {
            serial.close();
        }
    }
}
