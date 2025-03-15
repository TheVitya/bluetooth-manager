#include "BluetoothManager/device/BluetoothDevice.hpp"
#include "BluetoothManager/structs.hpp"
#include <cstdio>
#include "spdlog/spdlog.h"

BluetoothDevice::BluetoothDevice(const TDeviceConfiguration config)
    : config(std::move(config)) {
}

TStatusCode BluetoothDevice::connect() {
    status = DeviceStatus::CONNECTED;

    if (connectCallback) {
        connectCallback();
    }

    return TStatusCode::SUCCESS;
}

void BluetoothDevice::registerConnectCallback(TCallbackVoid callback) {
    connectCallback = callback;
}

TStatusCode BluetoothDevice::write(TPayload payload) {
    if (status == DeviceStatus::CONNECTED) {
        if (writeCallback) writeCallback(payload);
    } else {
        if (writeCallback && connect()) writeCallback(payload);
    }
    return TStatusCode::SUCCESS;
}

void BluetoothDevice::registerWriteCallback(TCallback<TPayload> callback) {
    writeCallback = callback;
}

TStatusCode BluetoothDevice::disconnect() {
    status = DeviceStatus::DISCONNECTED;
    if (disconnectCallback) disconnectCallback();
    return TStatusCode::SUCCESS;
}

void BluetoothDevice::registerDisconnectCallback(TCallbackVoid callback) {
    disconnectCallback = callback;
}

TStatusCode BluetoothDevice::pair() {
    status = DeviceStatus::PAIRED;
    if (pairCallback) pairCallback();
    return TStatusCode::SUCCESS;
}

void BluetoothDevice::registerPairCallback(TCallbackVoid callback) {
    pairCallback = callback;
}

TStatusCode BluetoothDevice::unpair() {
    status = DeviceStatus::DISCONNECTED;
    if (unpairCallback) unpairCallback();
    return TStatusCode::SUCCESS;
}

void BluetoothDevice::registerUnpairCallback(TCallbackVoid callback) {
    unpairCallback = callback;
}

TStatusCode BluetoothDevice::handleMessage(TPayload payload) {
    spdlog::info("Handling message");
    return TStatusCode::SUCCESS;
}
