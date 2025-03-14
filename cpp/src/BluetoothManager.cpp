#include "BluetoothManager/BluetoothManager.hpp"

std::shared_ptr<BluetoothManager> BluetoothManager::s_instance = nullptr;

void BluetoothManager::initialize() {
  if (!s_instance) {
    s_instance = std::shared_ptr<BluetoothManager>(new BluetoothManager());
  }
}

std::shared_ptr<BluetoothManager> BluetoothManager::instance() {
  if (!s_instance) {
    throw std::runtime_error("BluetoothManager: BluetoothManager::initialize must be called before BluetoothManager::instance.");
  }
  return s_instance;
}
