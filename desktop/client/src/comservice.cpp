#include "comservice.h"
#include <climits>

uint8_t COMService::extract(int start_bit, int bit_length)
{
    uint8_t data{0};

    int byte_index = start_bit / CHAR_BIT;
    int bit_index = start_bit % CHAR_BIT;

    {
        std::scoped_lock lock(buffer_mutex);

        for (int i = 0; i < bit_length; i++)
        {
            uint8_t bit = (buffer[byte_index] >> bit_index) & 0b00000001;

            data |= (bit << i);

            bit_index++;

            if (bit_index == CHAR_BIT)
            {
                bit_index = 0;
                byte_index++;
            }
        }
    }

    return data;
}

uint8_t COMService::get_speed(void)
{
    return extract(signal["speed"].bit_offset, signal["speed"].bit_size);
}

uint8_t COMService::get_battery(void)
{
    return extract(signal["battery"].bit_offset, signal["battery"].bit_size);
}
uint8_t COMService::get_left_light(void)
{
    return extract(signal["left_light"].bit_offset, signal["left_light"].bit_size);
}
uint8_t COMService::get_right_light(void)
{
    return extract(signal["right_light"].bit_offset, signal["right_light"].bit_size);
}

int8_t COMService::get_temperature(void)
{
    uint8_t data = extract(signal["temperature"].bit_offset, signal["temperature"].bit_size);
    uint8_t MSB_index = signal["temperature"].bit_size - 1;

    // check if the data is negativ
    if (data & (1 << MSB_index))
    {
        data |= ~((1 << (MSB_index + 1)) - 1); // set extended bits to 1
    }

    return static_cast<int8_t>(data);
}
