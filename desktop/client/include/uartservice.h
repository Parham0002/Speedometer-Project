#ifndef UARTCOM_H
#define UARTCOM_H

#include <QThread>
#include <QDebug>
#include "comservice.h"
#include "setting.h"

class UARTService : public COMService, public QThread
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