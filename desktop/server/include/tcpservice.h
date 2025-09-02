#ifndef TCPSERVICE_H
#define TCPSERVICE_H

#include "comservice.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <atomic>
#include <mutex>

class TCPService : public ComService
{
private:
    int server_fd;
    std::atomic<bool> end{false};
    std::thread worker_thread;

    void run() override;

public:
    TCPService();

    ~TCPService()
    {
        end = true;

        close(server_fd);

        if (worker_thread.joinable())
        {
            worker_thread.join();
        }
    }
};

#endif // Alex kod
