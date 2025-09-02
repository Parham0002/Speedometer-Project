#include "setting.h"
#include <QSerialPort>
#include "uartservice.h"
#include <QDebug>

void UARTService::run()
{
    QSerialPort serial;
    uint8_t temp[sizeof(buffer)]{0};

    serial.setPortName(UART_SPORT);
    serial.setBaudRate(BAUDRATE);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    while (!end)
    {
        if (serial.open(QIODevice::WriteOnly))
        {
            (void)serial.clear();

            while (!end && serial.isWritable())
            {
                {
                    std::scoped_lock<std::mutex> lock{buffer_mtx};
                    std::memcpy(temp, buffer, sizeof(buffer));
                }

                if (sizeof(temp) == serial.write(reinterpret_cast<const char *>(temp), sizeof(temp)))
                {
                    if (serial.flush())
                    {
                        status = true;
                        QThread::msleep(settings::DRAW_INTERVAL);
                    }
                    else
                    {
                        status = false;
                        qDebug() << "Error in communication";
                        break;
                    }
                }
                else
                {
                    status = false;
                    qDebug() << "Error in communication";
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
            status = false;
            serial.close();
        }
    }
}
