package org.BluetoothManager;

import static java.lang.Thread.sleep;

import android.util.Log;

public class BluetoothManagerJava {
    static {
        System.loadLibrary("BluetoothManager");
    }

    public static native void nativeInvoke(long nativePtr, byte[] response);

    public native void initialize();

    public native void createDevice(
        String address,
        DeviceOperationCallback deviceOperationCallback
    );

    public void setupDevice() {
        // Implement the callback in Java
        DeviceOperationCallback callback = new DeviceOperationCallback() {
            @Override
            public void onWrite(
                String address,
                int id,
                byte[] payload,
                Callback responseCallback
            ) {
                Log.d(
                    "RNBluetooth",
                    "Write callback received with id: " +
                    id +
                    " address " +
                    address
                );

                byte[] response = new byte[] { 1, 2, 3, 4 };
                responseCallback.invoke(response);
            }

            @Override
            public void onConnect(String address, Callback responseCallback) {
                Log.d(
                    "RNBluetooth",
                    "Connect callback received for address: " + address
                );
                byte[] response = new byte[] { 1, 2, 3, 4 };
                responseCallback.invoke(response);
            }

            @Override
            public void onDisconnect(
                String address,
                Callback responseCallback
            ) {
                Log.d(
                    "RNBluetooth",
                    "Disconnect callback received for address: " + address
                );
                byte[] response = new byte[] { 1, 2, 3, 4 };
                responseCallback.invoke(response);
            }
        };
        // Pass the callback to the native code
        createDevice("123", callback);
        createDevice("1234", callback);
        createDevice("1235", callback);
        createDevice("1236", callback);
    }
}
