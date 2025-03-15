#pragma once

#include <iostream>
#include <map>
#include <memory>
#include "BluetoothManager/device/BluetoothDevice.hpp"
#include "BluetoothManager/interfaces/IBluetoothManager.hpp"
#include "BluetoothManager/structs.hpp"

class BluetoothManager: public IBluetoothManager {
public:
    std::map<TAddress, std::shared_ptr<BluetoothDevice>> devices;

    BluetoothManager() = default;
    BluetoothManager& operator=(BluetoothManager const&) = delete;

    static std::shared_ptr<BluetoothManager> instance();

    static void initialize();

    void registerConnectCallback(TAddress address, TCallbackVoid connectCallback) override;
    void registerWriteCallback(TAddress address, TCallback<TPayload> writeCallback) override;
    void registerDisconnectCallback(TAddress address, TCallbackVoid disconnectCallback) override;
    void registerPairCallback(TAddress address, TCallbackVoid pairCallback) override;
    void registerUnpairCallback(TAddress address, TCallbackVoid unpairCallback) override;

    std::shared_ptr<BluetoothDevice> registerNewDevice(TAddress address);

private:
    static std::shared_ptr<BluetoothManager> s_instance;
    void registerDeviceCallback(TAddress address, std::function<void(BluetoothDevice&)> callback);
};
