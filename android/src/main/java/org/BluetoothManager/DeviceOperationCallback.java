package org.BluetoothManager;

public interface DeviceOperationCallback {
    void onWrite(
        String address,
        int id,
        byte[] payload,
        Callback responseCallback
    );

    void onConnect(String address, Callback responseCallback);

    void onDisconnect(String address, Callback responseCallback);
}
