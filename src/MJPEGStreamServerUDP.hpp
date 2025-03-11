/****************************************************************************
 * Copyright (C) 2016,2017 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef _UDPCLIENT_WINDOW_H_
#define _UDPCLIENT_WINDOW_H_

#define DEFAULT_UDP_CLIENT_PORT    9445

#include <iostream>
#include <thread>
#include <atomic>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "crc32.h"
#include "JpegInformation.h"
#include "MJPEGStreamServer.hpp"

class MJPEGStreamServerUDP : public MJPEGStreamServer {
public:
    ~MJPEGStreamServerUDP() {
        StopAsyncThread();
        if (sockfd != -1) {
            close(sockfd);
        }
    }

    static MJPEGStreamServerUDP* createInstance(int32_t ip, int32_t port) {
        return new MJPEGStreamServerUDP(ip, port);
    }

    void StartAsyncThread() {
        shouldExit = false;
        asyncThread = std::thread(&MJPEGStreamServerUDP::DoAsyncThreadInternal, this);
    }

    void StopAsyncThread() {
        shouldExit = true;
        if (asyncThread.joinable()) {
            asyncThread.join();
        }
    }

    void setThreadPriority(int priority) {
        // Optional: You can set the thread priority here if needed, depends on your platform.
    }

    void processData() {
        // This method will be called in a loop to handle the JPEG streaming.
        std::cout << "Processing data..." << std::endl;

        // Example JPEG Information for streaming.
        JpegInformation jpegInfo;
        jpegInfo.setFilename("example.jpg");

        // Stream JPEG
        streamJPEG(&jpegInfo);
    }

    bool streamJPEG(JpegInformation *info) {
        // This method will stream the JPEG data.
        std::cout << "Streaming JPEG: " << info->getFilename() << std::endl;

        // In a real-world scenario, you would convert info->getFilename() to an actual byte stream.
        uint8_t buffer[1024];  // Simulating a JPEG buffer for the sake of example.
        uint64_t size = sizeof(buffer);

        return sendJPEG(buffer, size);
    }

    void sendJPEG(uint8_t *buffer, uint64_t size) {
        // Send JPEG data over UDP.
        if (sockfd == -1) {
            std::cerr << "Socket not initialized!" << std::endl;
            return;
        }

        sockaddr_in destAddr;
        std::memset(&destAddr, 0, sizeof(destAddr));
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(DEFAULT_UDP_CLIENT_PORT);
        destAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Send to localhost for testing.

        int sentBytes = sendto(sockfd, buffer, size, 0, (struct sockaddr*)&destAddr, sizeof(destAddr));

        if (sentBytes < 0) {
            std::cerr << "Failed to send data" << std::endl;
        } else {
            std::cout << "Sent " << sentBytes << " bytes of JPEG data" << std::endl;
        }
    }

    bool sendData(uint8_t *data, int32_t length) {
        // Send generic data over UDP.
        if (sockfd == -1) {
            std::cerr << "Socket not initialized!" << std::endl;
            return false;
        }

        sockaddr_in destAddr;
        std::memset(&destAddr, 0, sizeof(destAddr));
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(DEFAULT_UDP_CLIENT_PORT);
        destAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // Send to localhost for testing.

        int sentBytes = sendto(sockfd, data, length, 0, (struct sockaddr*)&destAddr, sizeof(destAddr));

        if (sentBytes < 0) {
            std::cerr << "Failed to send data" << std::endl;
            return false;
        }
        std::cout << "Sent " << sentBytes << " bytes of data" << std::endl;
        return true;
    }

    void setSocket(int32_t sockfd) {
        this->sockfd = sockfd;
    }

private:
    MJPEGStreamServerUDP(uint32_t ip, int32_t port) {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "Failed to create socket" << std::endl;
            return;
        }

        sockaddr_in serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = htonl(ip);

        if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Failed to bind socket" << std::endl;
        }
    }

    void DoAsyncThreadInternal() {
        while (!shouldExit) {
            processData();
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep to simulate async work
        }
    }

    std::thread asyncThread;
    std::atomic<bool> shouldExit{false};
    int sockfd = -1;
};

#endif //_UDPCLIENT_WINDOW_H_
