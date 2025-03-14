#include <jni.h>
#include "BluetoothManager/BluetoothManager.hpp"
#include "BluetoothManager/device/Device.hpp"

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerJava_initialize(JNIEnv *env, jobject thiz) {
  BluetoothManager::initialize();
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerJava_createDevice(JNIEnv *env, jobject thiz, jstring address, jobject deviceOperationCallback) {
  BluetoothManager::initialize();

  const char *addressStr = env->GetStringUTFChars(address, nullptr);
  std::string addressCppStr(addressStr);
  env->ReleaseStringUTFChars(address, addressStr);

  // Store the callback reference
  jclass DEVICE_OPERATION_CALLBACK_CLASS = env->GetObjectClass(deviceOperationCallback);
  jmethodID writeMethodOnClass = env->GetMethodID(DEVICE_OPERATION_CALLBACK_CLASS, "onWrite",  "(Ljava/lang/String;I[BLorg/BluetoothManager/Callback;)V");
  jmethodID connectMethodOnClass = env->GetMethodID(DEVICE_OPERATION_CALLBACK_CLASS, "onConnect",  "(Ljava/lang/String;Lorg/BluetoothManager/Callback;)V");
  jmethodID disconnectMethodOnClass = env->GetMethodID(DEVICE_OPERATION_CALLBACK_CLASS, "onDisconnect",  "(Ljava/lang/String;Lorg/BluetoothManager/Callback;)V");

  Device device = Device(DeviceConfiguration {
      .address = addressStr,
      .writeCallback = [env, deviceOperationCallback, writeMethodOnClass](std::string address, int id, PayloadType& payload, CallbackType callback) {
          jbyteArray byteArray = env->NewByteArray(payload.size());

          std::vector<jbyte> byteData(payload.size());
          for (size_t i = 0; i < payload.size(); ++i) {
              byteData[i] = static_cast<jbyte>(payload[i]);
          }
          env->SetByteArrayRegion(byteArray, 0, byteData.size(), byteData.data());

          CallbackType* callbackPtr = &callback;

          jclass CALLBACK_CLASS = env->FindClass("org/BluetoothManager/Callback");
          jmethodID callbackConstructor = env->GetMethodID(CALLBACK_CLASS, "<init>", "(J)V");
          jobject callbackInstance = env->NewObject(CALLBACK_CLASS, callbackConstructor, reinterpret_cast<jlong>(callbackPtr));

          env->CallVoidMethod(deviceOperationCallback, writeMethodOnClass, env->NewStringUTF(address.c_str()), id, byteArray, callbackInstance);

          env->DeleteLocalRef(byteArray);
      },
      .connectCallback = [env, deviceOperationCallback, connectMethodOnClass](std::string address, CallbackType callback) {
          CallbackType* callbackPtr = &callback;

          jclass CALLBACK_CLASS = env->FindClass("org/BluetoothManager/Callback");
          jmethodID callbackConstructor = env->GetMethodID(CALLBACK_CLASS, "<init>", "(J)V");
          jobject callbackInstance = env->NewObject(CALLBACK_CLASS, callbackConstructor, reinterpret_cast<jlong>(callbackPtr));

          env->CallVoidMethod(deviceOperationCallback, connectMethodOnClass, env->NewStringUTF(address.c_str()), callbackInstance);
      },
      .disconnectCallback = [env, deviceOperationCallback, disconnectMethodOnClass](std::string address, CallbackType callback) {
          CallbackType* callbackPtr = &callback;

          jclass CALLBACK_CLASS = env->FindClass("org/BluetoothManager/Callback");
          jmethodID callbackConstructor = env->GetMethodID(CALLBACK_CLASS, "<init>", "(J)V");
          jobject callbackInstance = env->NewObject(CALLBACK_CLASS, callbackConstructor, reinterpret_cast<jlong>(callbackPtr));

          env->CallVoidMethod(deviceOperationCallback, disconnectMethodOnClass, env->NewStringUTF(address.c_str()), callbackInstance);
      }
  });

  auto cb1 = [](PayloadType p) {
  };

  device.addOperation(Operation {
      .callback = cb1
  });
  device.executeQueue();
}

extern "C"
JNIEXPORT void JNICALL
Java_org_BluetoothManager_BluetoothManagerJava_nativeInvoke(JNIEnv* env, jobject thiz, jlong nativePtr, jbyteArray response) {
    auto* callback = reinterpret_cast<CallbackType*>(nativePtr);

    if (callback) {
        jsize len = env->GetArrayLength(response);
        std::vector<byte> responseData(len);
        env->GetByteArrayRegion(response, 0, len, reinterpret_cast<jbyte*>(responseData.data()));

        (*callback)(responseData);

        delete callback;
    }
}
