#ifndef COMSERVICE_H
#define COMSERVICE_H

#include "setting.h"
#include <mutex>
#include <stdint.h>
#include <atomic>

class COMService
{
private:
    settings::Settings &signal{settings::Settings::getInstance()};

    uint8_t extract(int start_bit, int bit_length);

protected:
    uint8_t buffer[BUFFLEN]{};
    std::mutex buffer_mutex;
    std::atomic<bool> status{false};

public:
    uint8_t get_speed(void);
    int8_t get_temperature(void);
    uint8_t get_battery(void);
    uint8_t get_left_light(void);
    uint8_t get_right_light(void);
    bool get_status(void) { return status; }

    /// @brief Receive the buffer via any given communication protocol in derived classes.
    virtual void run() = 0;

    /// @brief Return true if there is a connection.
};

#endif