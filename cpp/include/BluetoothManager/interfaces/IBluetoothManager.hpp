#pragma once

#include "../structs.hpp"

class IBluetoothManager {
public:
    virtual void registerConnectCallback(TAddress address, TCallbackVoid connectCallback) = 0;
    virtual void registerWriteCallback(TAddress address, TCallback<TPayload> writeCallback) = 0;
    virtual void registerDisconnectCallback(TAddress address, TCallbackVoid disconnectCallback) = 0;
    virtual void registerPairCallback(TAddress address, TCallbackVoid pairCallback) = 0;
    virtual void registerUnpairCallback(TAddress address, TCallbackVoid unpairCallback) = 0;
};
