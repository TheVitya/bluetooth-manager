CMAKE_MINIMUM_REQUIRED(VERSION 3.10.2)

set(CMAKE_FOLDER "${CMAKE_CURRENT_LIST_DIR}/cmake")

include("${CMAKE_FOLDER}/variables.cmake")

option(MODULE_NAME "Name of the module" BluetoothManager)
option(BUILD_TARGET "The type of build" IOS) # EXECUTABLE, IOS, ANDROID

PROJECT("${MODULE_NAME}")

set(SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/cpp/src/BluetoothManager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/cpp/src/device/BluetoothDevice.cpp
)

set(HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/cpp/include/BluetoothManager/structs.hpp
  ${CMAKE_CURRENT_LIST_DIR}/cpp/include/BluetoothManager/BluetoothManager.hpp
  ${CMAKE_CURRENT_LIST_DIR}/cpp/include/BluetoothManager/interfaces/IBluetoothManager.hpp
  ${CMAKE_CURRENT_LIST_DIR}/cpp/include/BluetoothManager/interfaces/IBluetoothDevice.hpp
  ${CMAKE_CURRENT_LIST_DIR}/cpp/include/BluetoothManager/device/BluetoothDevice.hpp
  ${CMAKE_CURRENT_LIST_DIR}/cpp/include/BluetoothManager/device/BluetoothDeviceBase.hpp
)

add_library("${MODULE_NAME}" SHARED ${HEADERS} ${SOURCES})

target_include_directories("${MODULE_NAME}"
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/cpp/include
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/cpp/src
)

### DEPENDENCIES

include(FetchContent)

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.15.0
)

FetchContent_MakeAvailable(spdlog)
target_link_libraries("${MODULE_NAME}" spdlog)

###

if(BUILD_TARGET STREQUAL "EXECUTABLE")
  include("${CMAKE_FOLDER}/executable.cmake")
elseif(BUILD_TARGET STREQUAL "IOS")
  include("${CMAKE_FOLDER}/framework.cmake")
elseif(BUILD_TARGET STREQUAL "ANDROID")
  target_sources("${MODULE_NAME}" PUBLIC ${CMAKE_CURRENT_LIST_DIR}/android/src/main/cpp/cpp-adapter.cpp)
endif()
