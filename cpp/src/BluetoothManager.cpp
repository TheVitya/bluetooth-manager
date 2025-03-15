#include "BluetoothManager/BluetoothManager.hpp"
#include "BluetoothManager/device/BluetoothDevice.hpp"
#include "BluetoothManager/structs.hpp"
#include <memory>
#include "spdlog/spdlog.h"

#if defined(__ANDROID__)
#include "spdlog/sinks/android_sink.h"
#endif

std::shared_ptr<BluetoothManager> BluetoothManager::s_instance = nullptr;

void BluetoothManager::initialize() {
    if (!s_instance) {
        s_instance = std::make_shared<BluetoothManager>();

        #if defined(__ANDROID__)
        auto android_logger = spdlog::android_logger_mt("android", "BluetoothScanner");
        spdlog::set_default_logger(android_logger);
        #endif
    }
}

std::shared_ptr<BluetoothManager> BluetoothManager::instance() {
    if (!s_instance) {
        spdlog::error("BluetoothManager::initialize must be called before BluetoothManager::instance.");
        throw std::runtime_error("BluetoothManager::initialize must be called before BluetoothManager::instance.");
    }
    return s_instance;
}

void BluetoothManager::registerDeviceCallback(TAddress address, std::function<void(BluetoothDevice&)> callback) {
    auto& device = devices.try_emplace(address, std::make_unique<BluetoothDevice>(TDeviceConfiguration{.address = address})).first->second;
    callback(*device);
    spdlog::info("Registered callback for: {}", address);
}

void BluetoothManager::registerConnectCallback(TAddress address, TCallbackVoid connectCallback) {
    registerDeviceCallback(address, [&](BluetoothDevice& device) {
        device.registerConnectCallback(connectCallback);
    });
}

void BluetoothManager::registerWriteCallback(TAddress address, TCallback<TPayload> writeCallback) {
    registerDeviceCallback(address, [&](BluetoothDevice& device) {
        device.registerWriteCallback(writeCallback);
    });
}

void BluetoothManager::registerDisconnectCallback(TAddress address, TCallbackVoid disconnectCallback) {
    registerDeviceCallback(address, [&](BluetoothDevice& device) {
        device.registerDisconnectCallback(disconnectCallback);
    });
}

void BluetoothManager::registerPairCallback(TAddress address, TCallbackVoid pairCallback) {
    registerDeviceCallback(address, [&](BluetoothDevice& device) {
        device.registerPairCallback(pairCallback);
    });
}

void BluetoothManager::registerUnpairCallback(TAddress address, TCallbackVoid unpairCallback) {
    registerDeviceCallback(address, [&](BluetoothDevice& device) {
        device.registerUnpairCallback(unpairCallback);
    });
}

std::shared_ptr<BluetoothDevice> BluetoothManager::registerNewDevice(TAddress address) {
    auto device = std::make_shared<BluetoothDevice>(TDeviceConfiguration{.address = address});
    devices[address] = device;
    return device;
}
