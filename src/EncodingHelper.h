#ifndef __ENCODING_HELPER_H_
#define __ENCODING_HELPER_H_

#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>

#include "MJPEGStreamServerUDP.hpp"

#define ENCODE_QUEUE_MESSAGE_COUNT 1

class EncodingHelper {
public:
    static EncodingHelper* getInstance() {
        if (!instance) {
            instance = new EncodingHelper();
        }
        return instance;
    }

    static void destroyInstance() {
        if (instance) {
            instance->StopAsyncThread();
            delete instance;
            instance = nullptr;
        }
    }

    static bool addFSQueueMSG(int message) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (encodeQueue.size() < ENCODE_QUEUE_MESSAGE_COUNT) {
            encodeQueue.push(message);
            cv.notify_all();
            return true;
        }
        return false;
    }

    void StartAsyncThread() {
        shouldExit = false;
        asyncThread = std::thread(&EncodingHelper::DoAsyncThreadInternal, this);
    }

    void StopAsyncThread() {
        shouldExit = true;
        cv.notify_all();
        if (asyncThread.joinable()) {
            asyncThread.join();
        }
    }

    void setMJPEGStreamServer(MJPEGStreamServer* server) {
        this->mjpegServer = server;
    }

private:
    EncodingHelper() : shouldExit(false) {
        std::cout << "EncodingHelper initialized!" << std::endl;
    }

    void DoAsyncThreadInternal() {
        while (!shouldExit) {
            std::unique_lock<std::mutex> lock(queueMutex);
            cv.wait(lock, [this] { return !encodeQueue.empty() || shouldExit; });

            if (shouldExit) break;

            int message = encodeQueue.front();
            encodeQueue.pop();

            // Simulate encoding or processing logic
            std::cout << "Processing message: " << message << std::endl;

            if (mjpegServer) {
                // Assuming mjpegServer is used for streaming
                mjpegServer->processFrame(message);
            }
        }
    }

    static EncodingHelper* instance;
    std::thread asyncThread;
    std::atomic<bool> shouldExit;
    MJPEGStreamServer* mjpegServer = nullptr;

    static std::queue<int> encodeQueue;
    static std::mutex queueMutex;
    static std::condition_variable cv;
};

EncodingHelper* EncodingHelper::instance = nullptr;
std::queue<int> EncodingHelper::encodeQueue;
std::mutex EncodingHelper::queueMutex;
std::condition_variable EncodingHelper::cv;

#endif // __ENCODING_HELPER_H_
