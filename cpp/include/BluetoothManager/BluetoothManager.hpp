#pragma once

#include <iostream>

class BluetoothManager {
public:
  BluetoothManager() = default;
  BluetoothManager& operator=(BluetoothManager const&) = delete;

  static std::shared_ptr<BluetoothManager> instance();

  static void initialize();

private:
  static std::shared_ptr<BluetoothManager> s_instance;
};
