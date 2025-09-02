#include "comservice.h"
#include <climits>
void ComService::insert(int start_bit, int bit_length, uint32_t data)
{
    int byte_index{start_bit / CHAR_BIT};
    int bit_index{start_bit % CHAR_BIT};

    {
        std::scoped_lock lock(buffer_mtx);

        for (int i = 0; i < bit_length; i++)
        {
            uint8_t bit = (data >> i) & 1;

            if (bit == 0) // clear the bit at the position
            {
                buffer[byte_index] &= ~(1 << bit_index);
            }
            else // set the bit at the position
            {
                buffer[byte_index] |= bit << bit_index;
            }

            bit_index++;

            if (bit_index == CHAR_BIT)
            {
                bit_index = 0;
                byte_index++;
            }
        }
    }
}

void ComService::setSpeed(uint32_t speed)
{
    insert(signal["speed"].bit_offset, signal["speed"].bit_size, speed);
}

void ComService::setTemperature(int32_t temperature)
{
    insert(signal["temperature"].bit_offset, signal["temperature"].bit_size, temperature);
}

void ComService::setBatteryLevel(uint32_t level)
{
    insert(signal["battery"].bit_offset, signal["battery"].bit_size, level);
}

void ComService::setRightLight(bool state)
{
    insert(signal["right_light"].bit_offset, signal["right_light"].bit_size, static_cast<uint32_t>(state));
}

void ComService::setLeftLight(bool state)
{
    insert(signal["left_light"].bit_offset, signal["left_light"].bit_size, static_cast<uint32_t>(state));
}