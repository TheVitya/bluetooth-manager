#pragma once

#include <queue>
#include "BluetoothManager/interfaces/IBluetoothDevice.hpp"

class BluetoothDeviceBase : public IBluetoothDevice {
protected:
    std::queue<TOperation> operationQueue;

public:
    TStatusCode addOperation(TOperation operation) override {
        static std::atomic<int> counter{0};

        if (!operation.id) {
            operation.id = ++counter;
        }
        operationQueue.push(operation);
        return TStatusCode::SUCCESS;
    }

    TStatusCode processQueue() override {
        while (!operationQueue.empty()) {
            write(operationQueue.front().payload);
            operationQueue.pop();
        }
        return TStatusCode::SUCCESS;
    }
};
