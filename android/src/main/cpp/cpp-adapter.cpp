#include <jni.h>
#include <string>
#include "BluetoothManager/BluetoothManager.hpp"
#include "BluetoothManager/structs.hpp"

std::shared_ptr<BluetoothManager> instance;
static std::function<TStatusCode()> gPairCallback = nullptr;
static std::function<TStatusCode()> gUnpairCallback = nullptr;
static std::function<TStatusCode()> gConnectCallback = nullptr;
static std::function<TStatusCode()> gDisconnectCallback = nullptr;
static std::function<TStatusCode(TPayload)> gWriteCallback = nullptr;

std::string jstring2string(JNIEnv *env, jstring str) {
    std::string result;
    if (!str) return result;

    const char *kstr = env->GetStringUTFChars(str, nullptr);
    if (kstr) {
        std::string result(kstr);
        env->ReleaseStringUTFChars(str, kstr);
        return result;
    }
    return result;
}

// Helper function to call the Java callback and return a TStatusCode
TStatusCode invokeJavaCallback(JNIEnv *env, jobject callback, jstring address, std::optional<TPayload> payload = std::nullopt) {
    if (callback == nullptr) return TStatusCode::FAILED;

    // Get the callback's class
    jclass callbackClass = env->GetObjectClass(callback);
    jmethodID applyMethod = env->GetMethodID(callbackClass, "apply", "(Ljava/lang/String;[B)Lorg/BluetoothManager/StatusCode;");

    if (!applyMethod) {
        std::cerr << "Method apply(String, [B) not found!" << std::endl;
        return TStatusCode::FAILED;
    }

    // Convert payload if present, otherwise pass null
    jbyteArray byteArray = nullptr;
    if (payload.has_value() && !payload->empty()) {
        byteArray = env->NewByteArray(payload->size());
        env->SetByteArrayRegion(byteArray, 0, payload->size(), reinterpret_cast<const jbyte*>(payload->data()));
    }

    // Call the Java function, passing null if payload is not provided
    jobject result = env->CallObjectMethod(callback, applyMethod, address, byteArray);

    // Clean up local references
    if (byteArray) env->DeleteLocalRef(byteArray);

    // Convert the returned TStatusCode (Java enum) to an int
    jclass statusEnum = env->FindClass("org/BluetoothManager/StatusCode");
    jmethodID ordinalMethod = env->GetMethodID(statusEnum, "ordinal", "()I");
    int statusCode = env->CallIntMethod(result, ordinalMethod);

    return static_cast<TStatusCode>(statusCode);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_initialize(JNIEnv *env, jobject thiz) {
    BluetoothManager::initialize();
    instance = BluetoothManager::instance();
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_registerConnectCallback(JNIEnv *env, jclass clazz,
                                                                         jstring address,
                                                                         jobject callback) {
    std::string addStr = jstring2string(env, address);

    if (callback) {
        jobject gCallback = env->NewGlobalRef(callback);
        gConnectCallback = [env, gCallback, addStr]() -> TStatusCode {
            return invokeJavaCallback(env, gCallback, env->NewStringUTF(addStr.c_str()));
        };
    } else {
        gConnectCallback = nullptr;
    }

    instance->registerConnectCallback(jstring2string(env, address), gConnectCallback);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_registerWriteCallback(JNIEnv *env, jclass clazz,
                                                                       jstring address,
                                                                       jobject callback) {
    std::string addStr = jstring2string(env, address);

    if (callback) {
        jobject gCallback = env->NewGlobalRef(callback);
        gWriteCallback = [env, gCallback, addStr](TPayload payload) -> TStatusCode {
            // TODO
            return invokeJavaCallback(env, gCallback, env->NewStringUTF(addStr.c_str()), payload);
        };
    } else {
        gWriteCallback = nullptr;
    }

    instance->registerWriteCallback(jstring2string(env, address), gWriteCallback);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_registerDisconnectCallback(JNIEnv *env,
                                                                            jclass clazz,
                                                                            jstring address,
                                                                            jobject callback) {
    std::string addStr = jstring2string(env, address);

    if (callback) {
        jobject gCallback = env->NewGlobalRef(callback);
        gDisconnectCallback = [env, gCallback, addStr]() -> TStatusCode {
            return invokeJavaCallback(env, gCallback, env->NewStringUTF(addStr.c_str()));
        };
    } else {
        gDisconnectCallback = nullptr;
    }

    instance->registerDisconnectCallback(jstring2string(env, address), gDisconnectCallback);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_registerPairCallback(JNIEnv *env, jclass clazz,
                                                                      jstring address,
                                                                      jobject callback) {
    std::string addStr = jstring2string(env, address);

    if (callback) {
        jobject gCallback = env->NewGlobalRef(callback);
        gPairCallback = [env, gCallback, addStr]() -> TStatusCode {
            return invokeJavaCallback(env, gCallback, env->NewStringUTF(addStr.c_str()));
        };
    } else {
        gPairCallback = nullptr;
    }

    instance->registerPairCallback(addStr, gPairCallback);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_registerUnpairCallback(JNIEnv *env, jclass clazz,
                                                                        jstring address,
                                                                        jobject callback) {
    std::string addStr = jstring2string(env, address);

    if (callback) {
        jobject gCallback = env->NewGlobalRef(callback);
        gUnpairCallback = [env, gCallback, addStr]() -> TStatusCode {
            return invokeJavaCallback(env, gCallback, env->NewStringUTF(addStr.c_str()));
        };
    } else {
        gUnpairCallback = nullptr;
    }

    instance->registerUnpairCallback(jstring2string(env, address), gUnpairCallback);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_registerNewDevice(JNIEnv *env, jclass clazz,
                                                                   jstring address) {
    instance->registerNewDevice(jstring2string(env, address));
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_disconnect(JNIEnv *env, jobject thiz,
                                                            jstring address) {
    instance->devices[jstring2string(env, address)]->disconnect();
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_connect(JNIEnv *env, jobject thiz,
                                                         jstring address) {
    instance->devices[jstring2string(env, address)]->connect();
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_pair(JNIEnv *env, jobject thiz,
                                                      jstring address) {
    instance->devices[jstring2string(env, address)]->pair();
}
extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_unpair(JNIEnv *env, jobject thiz,
                                                        jstring address) {
    instance->devices[jstring2string(env, address)]->unpair();
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerKotlin_handleMessage(JNIEnv *env, jobject thiz,
                                                               jstring address,
                                                               jbyteArray payload) {
    jsize len = env->GetArrayLength(payload);
    std::vector<byte> byteData(len);
    env->GetByteArrayRegion(payload, 0, len, reinterpret_cast<jbyte *>(byteData.data()));

    instance->devices[jstring2string(env, address)]->handleMessage(byteData);
}
