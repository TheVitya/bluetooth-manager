#pragma once

#include "../structs.hpp"

class IBluetoothDevice {
public:
    virtual TStatusCode handleMessage(TPayload payload) = 0;

    virtual TStatusCode connect() = 0;
    virtual void registerConnectCallback(TCallbackVoid connectCallback) = 0;

    virtual TStatusCode disconnect() = 0;
    virtual void registerDisconnectCallback(TCallbackVoid disconnectCallback) = 0;

    virtual TStatusCode pair() = 0;
    virtual void registerPairCallback(TCallbackVoid pairCallback) = 0;

    virtual TStatusCode unpair() = 0;
    virtual void registerUnpairCallback(TCallbackVoid unpairCallback) = 0;

    virtual void registerWriteCallback(TCallback<TPayload> writeCallback) = 0;

    virtual TStatusCode addOperation(TOperation operation) = 0;
    virtual TStatusCode processQueue() = 0;
protected:
    virtual TStatusCode write(TPayload payload) = 0;
};
