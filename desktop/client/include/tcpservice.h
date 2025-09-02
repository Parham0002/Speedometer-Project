#ifndef TCPSERVICE_H
#define TCPSERVICE_H
#include "comservice.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <atomic>
#include <thread>
class TCPService : public COMService
{
private:
    int client_fd;
    struct sockaddr_in server_address;
    std::atomic<bool> end{false};
    std::thread worker_thread;

public:
    TCPService();
    ~TCPService()
    {
        end = true;
        if (worker_thread.joinable())
        {
            worker_thread.join();
        }
    }
    void run() override;
};
#endif