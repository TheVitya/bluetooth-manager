package org.BluetoothManager;

import android.util.Log;

public class Callback {

    private final long nativePtr;

    public Callback(long nativePtr) {
        this.nativePtr = nativePtr;
    }

    public void invoke(byte[] response) {
        BluetoothManagerJava.nativeInvoke(nativePtr, response);
    }
}
