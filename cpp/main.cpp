#include "BluetoothManager/BluetoothManager.hpp"
#include "BluetoothManager/structs.hpp"

// Example usage
int main() {
    BluetoothManager::initialize();

    auto manager = BluetoothManager::instance();
    auto device = manager->registerNewDevice("123");

    manager->registerConnectCallback("123", []() {
        return TStatusCode::SUCCESS;
    });
    device->connect();
    device->connect();

    return 0;
}
