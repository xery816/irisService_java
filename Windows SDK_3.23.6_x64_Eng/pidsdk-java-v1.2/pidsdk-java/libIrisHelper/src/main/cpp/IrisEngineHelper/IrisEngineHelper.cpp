#include <cstring>
#include <string>
#include <time.h>

#include <sstream>

#define  LOG_TAG  "IrisHelper"

#define  LOGD(...)  // __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  // __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
// If you want you can add other log definition for info, warning etc

#include "IrisEngineHelper.h"

typedef unsigned char BYTE;

#ifdef __cplusplus
extern "C" {
#endif

class _cbEnv_t {
public:
    JavaVM *jvm;
    jobject classLoader;
    jmethodID findClassMethod;
};

_cbEnv_t cbEnv;

static jbyteArray cache_data_ref = NULL;
static int cache_size = 0;

JNIEnv *
getEnv() {
    JNIEnv *env = nullptr;
    int status = cbEnv.jvm->GetEnv((void **) &env, JNI_VERSION_1_6);
    switch (status) {
        case JNI_EDETACHED: {
//            cbEnv.jvm->AttachCurrentThread((void **)&env, nullptr);
//            break;
        }
        case JNI_OK:
        case JNI_EVERSION:
        case JNI_VERSION_1_6:
        default:
            break;
    }
    return env;
}

void
freeEnv() {
//    cbEnv.jvm->DetachCurrentThread();
}

jobject irisHelperObj;

jclass
findIrisClass(JNIEnv *env, const char *name) {
//    jstring cls = env->NewStringUTF(name);
//    jclass c = (jclass) env->CallObjectMethod(cbEnv.classLoader, cbEnv.findClassMethod, cls);
    jclass c = env->GetObjectClass(irisHelperObj);
//    env->DeleteLocalRef(cls);
    return c;
}

// initiate function, call instantly when library loaded
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    memset(&cbEnv, 0, sizeof(_cbEnv_t));
    cbEnv.jvm = vm;

    JNIEnv *env = getEnv();

    jclass helperClz = env->FindClass("cn/simbok/irisHelper/IrisHelper");
    jclass helperClzClz = env->GetObjectClass(helperClz);

    auto clzLoaderClz = env->FindClass("java/lang/ClassLoader");
    auto getClzLoaderMethod = env->GetMethodID(helperClzClz, "getClassLoader",
                                               "()Ljava/lang/ClassLoader;");

    cbEnv.classLoader = env->NewGlobalRef(env->CallObjectMethod(helperClz, getClzLoaderMethod));
    cbEnv.findClassMethod = env->GetMethodID(clzLoaderClz, "findClass",
                                             "(Ljava/lang/String;)Ljava/lang/Class;");

    return JNI_VERSION_1_6;
}

void
cbHelperInit(const IrisEventInfo *iei) {
    JNIEnv *env = getEnv();

    jclass helperClz = findIrisClass(env, "cn/simbok/irisHelper/IrisHelper");

    // 1. kotlin: Get helper's static enroll callback function
    jmethodID method = env->GetStaticMethodID(helperClz, "cbInit", "(II)I");

    // 2. call the function wit
    env->CallStaticIntMethod(helperClz, method, (int)iei->event, iei->param);

    // 3. clean up
    env->DeleteLocalRef(helperClz);

//    if (result->finish == 1) {
//        freeEnv();
//    }
}

void
cbHelperEnroll(const char *name, iris_callback_result *result) {
    JNIEnv *env = getEnv();

    jclass helperClz = findIrisClass(env, "cn/simbok/irisHelper/IrisHelper");

    // 1. kotlin: Get helper's static enroll callback function
    jmethodID method = env->GetStaticMethodID(helperClz, "cbEnroll", "(Ljava/lang/String;III)I");

    // 2. call the function with (string(name), result)
    jstring s = env->NewStringUTF(name);
    env->CallStaticIntMethod(helperClz, method, s, result->result, result->eye, result->finish);
    env->DeleteLocalRef(s);

    // 3. clean up
    env->DeleteLocalRef(helperClz);
//    freeEnv();
}

void
cbHelperIdentify(const char *name, iris_callback_result *result) {
    JNIEnv *env = getEnv();

    jclass helperClz = findIrisClass(env, "cn/simbok/irisHelper/IrisHelper");

    // 1. kotlin: Get helper's static enroll callback function
    jmethodID method = env->GetStaticMethodID(helperClz, "cbIdentify", "(Ljava/lang/String;III)I");

    // 2. call the function with (string(name), result)
    jstring s = env->NewStringUTF(name);
    env->CallStaticIntMethod(helperClz, method, s, result->result, result->eye, result->finish);
    env->DeleteLocalRef(s);

    // 3. clean up
    env->DeleteLocalRef(helperClz);

    // freeEnv();
}

void
cbHelperPreview(const unsigned char *data, int len, int width, int height, int flag) {
    struct timespec ts_begin_1, ts_end_1;
    clock_gettime(CLOCK_MONOTONIC, &ts_begin_1);

    JNIEnv *env = getEnv();
    jclass helperClz = findIrisClass(env, "cn/simbok/irisHelper/IrisHelper");

    // 1. kotlin: Get helper's static preview callback function
    jmethodID method = env->GetStaticMethodID(helperClz, "cbPreview", "([BII)I");

    // 2. call the function with (string(name), result)
    if (cache_size < len || cache_data_ref == nullptr) {
        jbyteArray s = env->NewByteArray(len);

        if (cache_data_ref) {
            env->DeleteGlobalRef(cache_data_ref);
        }
        cache_data_ref = (jbyteArray) env->NewGlobalRef(s);
        cache_size = len;
        env->DeleteLocalRef(s);
    }
    //jbyteArray s = env->NewByteArray(len);
    env->SetByteArrayRegion(cache_data_ref, 0, len, (jbyte *) data);
    env->CallStaticIntMethod(helperClz, method, cache_data_ref, width, height);

    env->DeleteLocalRef(helperClz);
//    freeEnv();

    clock_gettime(CLOCK_MONOTONIC, &ts_end_1);
    long l_interval_1 = (ts_end_1.tv_sec - ts_begin_1.tv_sec) * 1000;
    l_interval_1 += (ts_end_1.tv_nsec - ts_begin_1.tv_nsec) / 1000 / 1000;

    LOGD("JNI_cbHelperPreview Whole Duration: %d ms", l_interval_1);
}

void
cbHelperPlaySound(const int a_id) {
    JNIEnv *env = getEnv();

    jclass helperClz = findIrisClass(env, "cn/simbok/irisHelper/IrisHelper");

    // 1. kotlin: Get helper's static enroll callback function
    jmethodID method = env->GetStaticMethodID(helperClz, "cbPlaySound", "(I)I");

    // 2. call the function with (string(name), result)
    env->CallStaticIntMethod(helperClz, method, a_id);

    // 3. clean up
    env->DeleteLocalRef(helperClz);
//    freeEnv();
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineInit(JNIEnv *env, jobject thiz,
                                                    jstring conf) {
    IrisEng_set_vm(cbEnv.jvm);
    irisHelperObj = env->NewGlobalRef(thiz);
    // int ret = IrisEng_initiate("{\"DIR_DATA\": \"/sdcard/irisengine/doorguard/data\", \"DIR_EYEDATA\":\"/sdcard/irisengine/doorguard/data/eyedata\", \"iris_device0\":\"/dev/video0\", \"PWM_HIGH_PST\":80, \"THD_FOCUS_RANGE\":\"40-80-90\"}");
    const char *engineConfig = env->GetStringUTFChars(conf, nullptr);
    int ret = IrisEng_initiate(engineConfig, cbHelperInit);
    IrisEng_set_sound_player(cbHelperPlaySound);

    return ret;
}

JNIEXPORT void JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineRelease(JNIEnv *env, jobject thiz) {
    if (cache_data_ref) {
        env->DeleteGlobalRef(cache_data_ref);
        cache_data_ref = nullptr;
    }

    IrisEng_release();
}

JNIEXPORT void JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineRelease2(JNIEnv *env, jobject thiz,
                                                        jboolean release_data) {
    if (release_data && cache_data_ref) {
        env->DeleteGlobalRef(cache_data_ref);
        cache_data_ref = nullptr;
    }

    IrisEng_release2(release_data);
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineEnroll(JNIEnv *env, jobject thiz,
                                                      jstring jstrName, jint cover) {
    const char *name = env->GetStringUTFChars(jstrName, nullptr);
    int status = IrisEng_enroll(name, cover, cbHelperEnroll);
    env->ReleaseStringUTFChars(jstrName, name);

    return status;
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineIdentify(JNIEnv *env, jobject thiz,
                                                        jstring jstrName, int continuous) {
    const char *name = jstrName != nullptr ? env->GetStringUTFChars(jstrName, nullptr) : nullptr;
    int status = IrisEng_identify(name, continuous, cbHelperIdentify);
    if (jstrName != nullptr) {
        env->ReleaseStringUTFChars(jstrName, name);
    }

    return status;
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineStop(JNIEnv *env, jobject thiz) {
    return IrisEng_stop();
}

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetUserList(JNIEnv *env, jobject thiz) {
    int num = 0;
    IrisEng_get_user_list(&num, nullptr);
    if (num == 0) {
        return env->NewStringUTF("");
    }

    char *user_list = (char *) calloc(num, IRIS_USER_ID_LENGTH);
    IrisEng_get_user_list(&num, user_list);

    return env->NewStringUTF(user_list);
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineDeleteUser(JNIEnv *env, jobject thiz,
                                                          jstring user) {
    const char *name = user != nullptr ? env->GetStringUTFChars(user, nullptr) : nullptr;
    IrisEng_delete_user(name, 1);

    if (name != nullptr) {
        env->ReleaseStringUTFChars(user, name);
    }

    return 0;
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineDeleteAllUser(JNIEnv *env, jobject thiz) {
    return IrisEng_delete_all_user(0);
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineSetPreview(JNIEnv *env,
                                                          jobject thi, jstring mode,
                                                          jint format, jint width, jint height,
                                                          jint marginX, jint marginY) {
    const char *mod = env->GetStringUTFChars(mode, nullptr);
    int r = IrisEng_set_preview2(mod, (IMAGE_PIXEL_FORMAT) format, width, height, marginX, marginY,
                                 cbHelperPreview);
    env->ReleaseStringUTFChars(mode, mod);

    return r;
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineStartPurePreview(
        JNIEnv *env, jobject thi) {
    return IrisEng_start_preview();
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineStopPurePreview(
        JNIEnv *env, jobject thi) {
    return IrisEng_stop_preview();
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineStopPreview(
        JNIEnv *env, jobject thi) {
    return IrisEng_set_preview("", (IMAGE_PIXEL_FORMAT) 0, 0, 0, nullptr);
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEnginePauseAction(JNIEnv *env, jobject thi) {
    return IrisEng_pauseAction();
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineResumeAction(JNIEnv *env, jobject thi) {
    return IrisEng_resumeAction();
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineChangeConfigure(JNIEnv *env,
                                                               jobject thi,
                                                               jstring config) {
    const char *c = env->GetStringUTFChars(config, nullptr);
    int r = IrisEng_change_configure(c);
    env->ReleaseStringUTFChars(config, c);

    return r;
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineLoadDevParams(JNIEnv *env,
                                                             jobject thi,
                                                             jstring config) {
    const char *c = env->GetStringUTFChars(config, nullptr);
    int r = IrisEng_load_dev_params(c);
    env->ReleaseStringUTFChars(config, c);

    return r;
}

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineAddIrisUser(JNIEnv *env,
                                                           jobject thi, jstring uid,
                                                           jbyteArray left_iris,
                                                           jbyteArray right_iris) {
    const char *u = env->GetStringUTFChars(uid, nullptr);

    jbyte *left = left_iris == nullptr ? nullptr : env->GetByteArrayElements(left_iris, nullptr);
    jbyte *right = right_iris == nullptr ? nullptr : env->GetByteArrayElements(right_iris, nullptr);

    int ret = IrisEng_add_user(u, (unsigned char *) left, (unsigned char *) right, 0);

    // release
    env->ReleaseStringUTFChars(uid, u);
    if (left_iris != nullptr) env->ReleaseByteArrayElements(left_iris, left, 0);
    if (right_iris != nullptr) env->ReleaseByteArrayElements(right_iris, right, 0);

    return ret;
}

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetDeviceInfo2(JNIEnv *env, jobject thi) {
    char result[STRLEN];// = new char[1024];
    jstring r = nullptr;
    if (IrisEng_get_device_info_2(result, STRLEN) == 0) {
        r = env->NewStringUTF(result);
    }

//    delete[] result;

    return r;
}

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetEngineInfo(JNIEnv *env, jobject thiz) {
    std::stringstream info;
    info << "{";

    char ver[STRLEN];// = new char[1024];
    char date[STRLEN]; // = new char[1024];
    if (IrisEng_get_algor_version(ver, date) == 0) {
        info << R"("algorithm": ")" << ver << R"(", "algorithm_date": ")" << date << "\"";
    }
//    delete[] ver;
//    delete[] date;

    char ver1[STRLEN]; // = // new char[1024];
    char date1[STRLEN]; //= new char[1024];
    if (IrisEng_get_sdk_version(ver1, date1) == 0) {
        info << R"(, "sdk": ")" << ver1 << R"(", "sdk_date": ")" << date1 << "\"";
    }
//    delete[] ver1;
//    delete[] date1;

    char ver2[STRLEN];// = new char[1024];
    char date2[STRLEN]; // = new char[1024];
    if (IrisEng_get_dev_firmware_version(ver2, date2) == 0) {
        info << R"(, "firmware": ")" << ver2 << R"(", "firmware_date": ")" << date2 << "\"";
    }

//    delete[] ver2;
//    delete[] date2;

    info << "}";

    jstring r = env->NewStringUTF(info.str().c_str());

    return r;
}

JNIEXPORT jbyteArray JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetIrisCode(JNIEnv *env, jobject thiz,
                                                           jbyteArray frame,
                                                           jint size,
                                                           jstring format
) {
    int CODE_SIZE = 1500;

    const char *fmt = env->GetStringUTFChars(format, nullptr);
    BYTE *bf = reinterpret_cast<BYTE *>(env->GetByteArrayElements(frame, nullptr));

//    BYTE *code = static_cast<BYTE *>(malloc(CODE_SIZE));
//    BYTE *code = new BYTE[CODE_SIZE];
    BYTE code[CODE_SIZE]; // = new BYTE[CODE_SIZE];
    int r = IrisEng_extract_iris_code(bf, size, fmt, (BYTE *) code);

    jbyteArray result = nullptr;
    if (r == 0) {
        result = env->NewByteArray(CODE_SIZE);
        env->SetByteArrayRegion(result, 0, CODE_SIZE, (jbyte *) code);
    }

//    free(code);

//    delete[] code;

    return result;
}

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetEnrollData(JNIEnv *env, jobject thiz,
                                                             jstring uid) {
    const char *user_id = env->GetStringUTFChars(uid, nullptr);
    int data_len = IrisEng_get_enroll_data_len(user_id) + 1;

//    char *data = new char[data_len];
    char data[data_len]; // = new char[data_len];
    int r = IrisEng_fetch_enroll_data(user_id, data, data_len);

    if (r != 0) {
//        delete[] data;
        return nullptr;
    }

    jstring result = env->NewStringUTF(data);

//    delete[] data;

    return result;
}

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetIdentifyData(JNIEnv *env, jobject thiz) {
    int data_len = IrisEng_get_snap_data_len() + 1;

//    char *data = new char[data_len];
    char data[data_len];
    int r = IrisEng_fetch_snap_data(data, data_len);

    if (r != 0) {
//        delete[] data;
        return nullptr;
    }

    jstring result = env->NewStringUTF(data);

//    delete[] data;

    return result;
}

JNIEXPORT jint JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineUpdateFirmware(JNIEnv *env, jobject thiz,
                                                              jbyteArray firmware) {
    unsigned char *fw = (unsigned char *) env->GetByteArrayElements(firmware, nullptr);

    int len = env->GetArrayLength(firmware);

    int r = IrisEng_update_firmware(fw, len);

    return r;
}

void
cbCheckHardware(const char *name, iris_callback_result *result) {
    JNIEnv *env = getEnv();

    jclass helperClz = findIrisClass(env, "cn/simbok/irisHelper/IrisHelper");

    // 1. kotlin: Get helper's static check hardware callback function
    jmethodID method = env->GetStaticMethodID(helperClz, "cbCheckHardware",
                                              "(Ljava/lang/String;III)I");

    // 2. call the function with (string(name), result)
    jstring s = env->NewStringUTF(name);
    env->CallStaticIntMethod(helperClz, method, nullptr, result->result, result->eye,
                             result->finish);
    env->DeleteLocalRef(s);

    // 3. clean up
    env->DeleteLocalRef(helperClz);
//    freeEnv();
}

JNIEXPORT jint JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineCheckHardware(JNIEnv *env, jobject thiz) {
    int r = IrisEng_sanity_check(cbCheckHardware);

    return r;
}

#ifdef __cplusplus
};
#endif
