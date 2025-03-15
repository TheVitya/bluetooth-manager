#pragma once

#include <iostream>
#include <queue>
#include <future>
#include <optional>
#include <cstdio>
#include "BluetoothManager/device/BluetoothDeviceBase.hpp"
#include "BluetoothManager/structs.hpp"

enum DeviceStatus {
    FOUND,
    PAIRING,
    PAIRED,
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
    TIMEOUTED,
};

const static int OPERATION_TIMEOUT_SECONDS = 2;

class BluetoothDevice: public BluetoothDeviceBase {
public:
    const TDeviceConfiguration config;
    BluetoothDevice(const TDeviceConfiguration config);

    TCallbackVoid connectCallback;
    TStatusCode connect() override;
    void registerConnectCallback(TCallbackVoid connectCallback) override;

    TCallbackVoid disconnectCallback;
    TStatusCode disconnect() override;
    void registerDisconnectCallback(TCallbackVoid disconnectCallback) override;

    TCallbackVoid pairCallback;
    TStatusCode pair() override;
    void registerPairCallback(TCallbackVoid pairCallback) override;

    TCallbackVoid unpairCallback;
    TStatusCode unpair() override;
    void registerUnpairCallback(TCallbackVoid unpairCallback) override;

    void registerWriteCallback(TCallback<TPayload> writeCallback) override;

    TStatusCode handleMessage(TPayload payload) override;
private:
    DeviceStatus status;

    TCallback<TPayload> writeCallback;
    TStatusCode write(TPayload payload) override;
};
