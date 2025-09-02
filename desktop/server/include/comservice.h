#ifndef COMSERVICE_H
#define COMSERVICE_H

#include <cstdint>
#include "setting.h"
#include <mutex>
#include <atomic>

class ComService
{
    settings::Settings &signal{settings::Settings::getInstance()};
    void insert(int start, int length, uint32_t value);

protected:
    std::mutex buffer_mtx;
    uint8_t buffer[BUFFLEN]{0};
    std::atomic<bool> status{false};
    virtual void run(void) = 0;

public:
    // bool getStatus(void) { return status; }

    void setTemperature(int32_t temperature);
    void setBatteryLevel(uint32_t level);
    void setRightLight(bool state);
    void setLeftLight(bool state);
    void setSpeed(uint32_t speed);

    virtual ~ComService() = default;
};

#endif
