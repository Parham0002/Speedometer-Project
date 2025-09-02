#include "tcpservice.h"
#include <iostream>
#include <unistd.h>
#include <cstring>

TCPService::TCPService() : COMService()
{
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(settings::Server::PORT);
    if (inet_pton(AF_INET, settings::Server::IP_ADRESS, &server_address.sin_addr) <= 0)
    {
        exit(EXIT_FAILURE);
    }
    worker_thread = std::thread(&TCPService::run, this);
}
void TCPService::run()
{
    while (!end)
    {
        if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // create at socket for IPv4 and TCP
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) // connects the socket with the server
        {
            close(client_fd);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        status = true;

        while (!end)
        {
            uint8_t temp_buffer[sizeof(buffer)];
            int number_of_bytes_read = read(client_fd, temp_buffer, sizeof(temp_buffer));
            if (number_of_bytes_read <= 0)
            {
                close(client_fd);
                status = false;
                break;
            }
            else
            {
                std::scoped_lock lock(buffer_mutex);
                std::memcpy(buffer, temp_buffer, sizeof(buffer));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }
    if (client_fd >= 0)
    {
        close(client_fd);
        client_fd = -1;
        status = false;
    }
}
