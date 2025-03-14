#pragma once

#include "BluetoothManager/device/Device.hpp"

Device::Device(const DeviceConfiguration config)
    : config(std::move(config)) {
}

bool Device::addOperation(Operation operation) {
    static std::atomic<int> counter{0};

    if (!operation.id) {
        operation.id = ++counter;
    }

    operationQueue.push(operation);

    return true;
}

bool Device::executeQueue() {
    if (inProcessingQueue) return false;
    inProcessingQueue = true;

    if (status != DeviceStatus::CONNECTED && !connect()) {
        inProcessingQueue = false;
        return false;
    }

    lockAndWaitForResponse([this] {
        while (!operationQueue.empty()) {
            Operation& operation = operationQueue.front();
            config.writeCallback(config.address, operation.id.value(), operation.payload, operation.callback);
            operationQueue.pop();
        }

        notifyResponseReceived();
    });

    if (config.shouldDisconnectOnExecution) {
        disconnect();
    }

    inProcessingQueue = false;

    return true;
}

bool Device::disconnect() {
    return lockAndWaitForResponse([this] {
        config.disconnectCallback(config.address, [this] (PayloadType responseCallback) {
            notifyResponseReceived();
        });
    });
}

bool Device::connect() {
    return lockAndWaitForResponse([this] {
        config.connectCallback(config.address, [this] (PayloadType responseCallback) {
            notifyResponseReceived();
        });
    });
}

bool Device::lockAndWaitForResponse(std::function<void()> callback) {
    std::unique_lock<std::mutex> lock(operationMtx);
    response_received = false;

    callback();

    return cv.wait_for(lock, std::chrono::seconds(OPERATION_TIMEOUT_SECONDS), [this] { return response_received; });
}

void Device::notifyResponseReceived() {
    response_received = true;
    cv.notify_all();
}
