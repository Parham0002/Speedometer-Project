#ifndef UARTCOM_H
#define UARTCOM_H

#include "comservice.h"
#include <QThread>
#include <QSerialPort>

class UARTService : public ComService, public QThread
{
    std::atomic<bool> end{false};

    void run() override;

public:
    UARTService()
    {
        start();
    }

    ~UARTService()
    {
        end = true;
        wait();
    }
};

#endif
