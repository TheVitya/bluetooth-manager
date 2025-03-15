package org.BluetoothManager

fun interface Callback {
    fun apply(address: String, payload: ByteArray?): StatusCode
}
