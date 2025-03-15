package org.BluetoothManager

import android.Manifest
import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

class BluetoothManagerKotlin(private val context: Context) : Activity() {
    private val handler = Handler(Looper.getMainLooper())
    private var isScanning = false
    private val scanPeriod: Long = 10000 // Scan for 10 seconds

    private val bluetoothManager: BluetoothManager =
        context.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
    private val bluetoothAdapter: BluetoothAdapter? = bluetoothManager.adapter

    val DEVICES: HashMap<String, BluetoothDevice> = HashMap()

    init {
        System.loadLibrary("BluetoothManager")
        initialize()
    }

    private external fun initialize()

    external fun disconnect(address: String)

    external fun connect(address: String)

    external fun pair(address: String)

    external fun unpair(address: String)

    external fun handleMessage(address: String, payload: ByteArray)

    private external fun registerConnectCallback(
        address: String,
        connectCallback: Callback
    )

    private external fun registerWriteCallback(
        address: String,
        writeCallback: Callback
    )

    private external fun registerDisconnectCallback(
        address: String,
        disconnectCallback: Callback
    )

    private external fun registerPairCallback(
        address: String,
        pairCallback: Callback
    )

    private external fun registerUnpairCallback(
        address: String,
        unpairCallback: Callback
    )

    private external fun registerNewDevice(address: String)

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)

        if (requestCode == 1) { // Check the request code
            var allPermissionsGranted = true
            for (result in grantResults) {
                if (result != PackageManager.PERMISSION_GRANTED) {
                    allPermissionsGranted = false
                    break
                }
            }

            if (allPermissionsGranted) {
                Log.d("Bluetooth", "All permissions granted, you can scan for Bluetooth devices.")
            } else {
                Log.e("Bluetooth", "Some permissions were not granted.")
            }
        }
    }

    fun requestBluetoothPermissions(): Boolean {
        val permissions = mutableListOf<String>()

        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.BLUETOOTH
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            permissions.add(Manifest.permission.BLUETOOTH)
        }
        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.BLUETOOTH_ADMIN
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            permissions.add(Manifest.permission.BLUETOOTH_ADMIN)
        }
        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.BLUETOOTH_SCAN
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            permissions.add(Manifest.permission.BLUETOOTH_SCAN)
        }
        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.BLUETOOTH_CONNECT
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            permissions.add(Manifest.permission.BLUETOOTH_CONNECT)
        }
        if (ContextCompat.checkSelfPermission(
                context,
                Manifest.permission.ACCESS_FINE_LOCATION
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            permissions.add(Manifest.permission.ACCESS_FINE_LOCATION)
        }

        if (permissions.isNotEmpty() && context is Activity) {
            ActivityCompat.requestPermissions(context, permissions.toTypedArray(), 1)
            return false
        }

        return true
    }

    private val handleConnect: (String, ByteArray?) -> StatusCode =
        { address: String, payload: ByteArray? ->
            Log.d("BluetoothScanner", "Trying to connect $address")
            StatusCode.SUCCESS
        }

    private val handlePair: (String, ByteArray?) -> StatusCode =
        { address: String, payload: ByteArray? ->
            Log.d("BluetoothScanner", "Trying to pair $address")

            val device: BluetoothDevice = bluetoothAdapter!!.getRemoteDevice(address)

            if (device.bondState == BluetoothDevice.BOND_BONDING) {
                StatusCode.SUCCESS
            }
            device.createBond()
            StatusCode.SUCCESS
        }


    private val handleWrite: (String, ByteArray?) -> StatusCode =
        { address: String, payload: ByteArray? ->
            Log.d("BluetoothScanner", "Trying to write $payload")
            StatusCode.SUCCESS
        }

    private val handleUnpair:
                (String, ByteArray?) -> StatusCode =
        { address: String, payload: ByteArray? ->
            Log.d("BluetoothScanner", "Trying to unpair $address")

            val device: BluetoothDevice = bluetoothAdapter!!.getRemoteDevice(address)

            if (device.bondState == BluetoothDevice.BOND_BONDED) {
                try {
                    // Attempt to remove the bond (unpair)
                    val removeBondMethod =
                        device.javaClass.getMethod("removeBond")
                    val succeeded = removeBondMethod.invoke(device) as Boolean

                } catch (e: Exception) {

                }
            }
            StatusCode.SUCCESS
        }


    private val handleDisconnect: (String, ByteArray?) -> StatusCode =
        { address: String, payload: ByteArray? ->
            Log.d("BluetoothScanner", "Trying to handleDisconnect")
            StatusCode.SUCCESS
        }

    private val receiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {
            when (intent?.action) {
                BluetoothDevice.ACTION_FOUND -> {
                    val device: BluetoothDevice? =
                        intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE)

                    device?.let {
                        DEVICES[it.address] = device

                        registerNewDevice(it.address)
                        registerConnectCallback(it.address, handleConnect)
                        registerPairCallback(it.address, handlePair)
                        registerWriteCallback(it.address, handleWrite)
                        registerUnpairCallback(it.address, handleUnpair)
                        registerDisconnectCallback(it.address, handleDisconnect)
//                        pair(it.address)
//                        unpair(it.address)
                        disconnect(it.address)
                        connect(it.address)

                        val payload = ByteArray(3)
                        payload.fill(0, 0, 3)
                        handleMessage(it.address, payload)

                        Log.d(
                            "BluetoothScanner",
                            "Classic Bluetooth Device found: ${it.name} - ${it.address}"
                        )
                    }
                }

                BluetoothAdapter.ACTION_DISCOVERY_FINISHED -> {
                    Log.d("BluetoothScanner", "Classic Bluetooth Discovery finished")
                    context?.unregisterReceiver(this)
                }
            }
        }
    }

    fun startScan() {
        Log.d("BluetoothScanner", "Bluetooth is about to start syncing")

        if (isScanning || bluetoothAdapter == null || !bluetoothAdapter.isEnabled) {
            Log.e("BluetoothScanner", "Bluetooth is not enabled or already scanning")
            return
        }

        if (!requestBluetoothPermissions()) {
            Log.e("BluetoothScanner", "Permissions are not granted!")
            return
        }

        isScanning = true

        // Register BroadcastReceiver for Classic Bluetooth devices
        val filter = IntentFilter(BluetoothDevice.ACTION_FOUND)
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED)
        context.registerReceiver(receiver, filter)

        // Start Classic Bluetooth discovery
        bluetoothAdapter.startDiscovery()

        // Stop scanning after the predefined period
        handler.postDelayed({
            scanStop()
        }, scanPeriod)

        Log.d("BluetoothScanner", "Scanning started...")
    }

    fun scanStop() {
        if (!isScanning) return

        isScanning = false

        // Stop Classic Bluetooth discovery
        bluetoothAdapter?.cancelDiscovery()

        // Unregister the BroadcastReceiver
        // TODO: fix crash
//        context.unregisterReceiver(receiver)

        Log.d("BluetoothScanner", "Scanning stopped.")
    }
}
