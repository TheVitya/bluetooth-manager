#pragma once

#include <vector>
#include <functional>

typedef unsigned char byte;
using TPayload = const std::vector<byte>;

enum TStatusCode {
    SUCCESS,
    FAILED,
};

template <typename TPayload>
using TCallback = std::function<TStatusCode(TPayload)>;
using TCallbackVoid = std::function<TStatusCode(void)>;
using TCallbackResponse = std::function<TPayload(void)>;

using TAddress = std::string;

struct TOperation {
    // TCallback callback;
    std::optional<int> id;
    TPayload payload;
};

struct TDeviceConfiguration {
    std::string address;
    bool shouldDisconnectOnExecution = false;
};
