/**
 * @file setting.h
 * @brief
 * @version 0.1
 * @date 2025-06-09
 *
 * @copyright Copyright (c) 2025
 *
 *
 * This file is part of the shared library for the project.
 * It defines the Settings class, which provides access to signal values
 * using a subscript operator. The signals are defined in the SIGNALS macro.
 * The Settings class is a singleton, ensuring that only one instance exists
 * throughout the application. The signal values are stored in a map for
 * efficient access.
 *
 * how to use:
 * ```cpp
 * #include "setting.h"
 * using namespace settings;
 * Signal& settings = Signal::getInstance();
 * const signal_value& speed = settings["speed"];
 * std::cout << "Speed Bit Size: " << speed.bit_size << std::endl;
 * ```
 *
 * or
 *
 * ```cpp
 * #include "setting.h"
 * using namespace settings;
 * Signal& settings = Signal::getInstance();
 *
 * // Accessing signal values using subscript operator
 * int update_speed = settings["speed"].bit_size;
 * std::cout << "Speed Bit Size: " << update_speed << std::endl;
 *
 *
 * int update_speed_max(settings["speed"].max);
 * std::cout << "Speed Max: " << update_speed_max << std::endl;
 * ```
 *
 *  note: the key is case-sensitive, so "speed" and "Speed" are different keys.
 *
 *  return value is a reference to the signal_value struct, which contains the following
 * atributes:
 * - bit_size: the size of the signal in bits
 * - bit_offset: the offset of the signal in bits
 * - min: the minimum value of the signal
 * - max: the maximum value of the signal
 *
 */

#ifndef SETTING_H
#define SETTING_H

// overload subscript
// value, key
#define SIGNALS {                     \
    {{8, 0, 0, 240}, "speed"},        \
    {{7, 8, -60, 60}, "temperature"}, \
    {{7, 15, 0, 100}, "battery"},     \
    {{1, 22, 0, 1}, "left_light"},    \
    {{1, 23, 0, 1}, "right_light"}}

#define BUFFLEN 3
#define BAUDRATE 1048576

#ifdef __cplusplus

#include <string>
#include <map>
#include <stdexcept>

namespace settings
{

    constexpr int DRAW_INTERVAL{40};
    constexpr int BLINK_INTERVAL{350};

    struct signal_value
    {
        int bit_size;
        int bit_offset;
        int min;
        int max;

        constexpr signal_value(int bit_size, int bit_offset, int min, int max)
            : bit_size(bit_size), bit_offset(bit_offset), min(min), max(max) {}
    };

    struct signal_type
    {
        signal_value value;
        const char *key;

        constexpr signal_type(signal_value value, const char *key)
            : value(value), key(key) {}
    };

    class Settings
    {
    public:
        /**
         * @brief Get the singleton instance of Settings
         *
         * @return Settings&
         */
        static Settings &getInstance()
        {
            static Settings instance;
            return instance;
        }

        /**
         * @brief get the signal value by key using subscript operator
         *
         * @param key the key of the signal
         * @return const signal_value&
         */
        const signal_value &operator[](const char *key) const
        {
            auto it = signalMap.find(key);
            if (it != signalMap.end())
            {
                return it->second;
            }
            throw std::out_of_range("Signal key not found");
        }

        // Delete copy constructor and assignment operator to enforce singleton pattern
        Settings(const Settings &) = delete;
        void operator=(const Settings &) = delete;

        static Settings &handle()
        {
            return getInstance();
        }

    private:
        // Private constructor to prevent instantiation
        Settings()
        {
            // Initialize the signal map
            for (const auto &signal : signal_list)
            {
                signalMap.insert({signal.key, signal.value});
            }
        };

        constexpr static signal_type signal_list[] = SIGNALS;
        std::map<const std::string, const signal_value> signalMap;
    };
    namespace Server
    {
        inline constexpr const char *IP_ADRESS{"127.0.0.1"};
        inline constexpr int PORT{8080};
    }

    namespace SerialPort
    {
        inline constexpr int WAIT_FOR_DATA_MS{100};
    }

    namespace Config
    {
        inline constexpr int SERVER_WINDOW_WIDTH{800};
        inline constexpr int SERVER_WINDOW_HEIGHT{150};

        inline constexpr int CLIENT_WINDOW_WIDTH{800};
        inline constexpr int CLIENT_WINDOW_HEIGHT{560};

        inline constexpr int OFFSET{38}; // The task bar at the top
    };
}
#endif

#endif
