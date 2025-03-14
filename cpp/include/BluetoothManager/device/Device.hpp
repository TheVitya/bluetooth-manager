#pragma once

#include <iostream>
#include <queue>
#include <future>
#include <optional>
#include <cstdio>

typedef unsigned char byte;
using PayloadType = const std::vector<byte>;
using CallbackType = std::function<void(PayloadType)>;

struct DeviceConfiguration {
    std::string address;
    bool shouldDisconnectOnExecution = false;

    /// used for communication between platform native and cpp side
    std::function<void(std::string address, int operationId, PayloadType& payload, CallbackType responseCallback)> writeCallback;
    std::function<void(std::string address, CallbackType responseCallback)> connectCallback;
    std::function<void(std::string address, CallbackType responseCallback)> disconnectCallback;
};

struct Operation {
    CallbackType callback;
    std::optional<int> id;
    PayloadType payload;
};

enum DeviceStatus {
    FOUND,
    PAIRING,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
    TIMEOUTED,
};

const static int OPERATION_TIMEOUT_SECONDS = 2;

class Device {
public:
    const DeviceConfiguration config;
    Device(const DeviceConfiguration config);

    bool executeQueue();
    bool addOperation(Operation operation);

    bool connect();
    bool disconnect();
    bool lockAndWaitForResponse(std::function<void()> callback);
private:
    DeviceStatus status;
    std::queue<Operation> operationQueue;
    bool inProcessingQueue = false;

    std::mutex operationMtx;
    std::condition_variable cv;
    bool response_received = false;

    void notifyResponseReceived();
};
