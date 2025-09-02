#include <cstdlib>
#include <cstring>
#include "tcpservice.h"
#include <iostream>

TCPService::TCPService() : ComService()
{
    static constexpr int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (0 > server_fd)
    {
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(settings::Server::PORT);

    if (0 > bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)))
    {
        exit(EXIT_FAILURE);
    }

    if (0 > listen(server_fd, 3))
    {
        exit(EXIT_FAILURE);
    }

    worker_thread = std::thread(&TCPService::run, this);
}

void TCPService::run()
{
    int connection_fd;
    while (!end)
    {
        sockaddr_in client_addr{};
        socklen_t len{sizeof(client_addr)};

        connection_fd = accept(server_fd, (sockaddr *)&client_addr, &len);

        if (0 > connection_fd)
        {
            status = false;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        status = true;

        uint8_t temp_buffer[BUFFLEN]{};

        while (!end)
        {
            {
                std::scoped_lock lock(buffer_mtx);
                std::memcpy(temp_buffer, buffer, BUFFLEN);
            }
            if (BUFFLEN == send(connection_fd, temp_buffer, BUFFLEN, 0))
            {
                status = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
            else
            {
                std::cout << "Connection lost" << std::endl;
                status = false;
                break;
            }
        }
        shutdown(connection_fd, SHUT_RDWR);
        close(connection_fd);
    }
}
