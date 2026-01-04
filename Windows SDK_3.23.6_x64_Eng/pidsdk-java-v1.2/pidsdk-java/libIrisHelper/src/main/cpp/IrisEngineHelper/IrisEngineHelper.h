//
// Created by Huang caiting on 2019/3/13.
//

#ifndef IRISENGINEDEMO_IrisHelper_H
#define IRISENGINEDEMO_IrisHelper_H


//#include <_mingw.h>
//#include <stdint.h>
//#define __int64 int64_t
//typedef long __int64;

#include <jni.h>

//#include "irisengine_export.h"
#include <irisengine_export.h>
//#include "irisengine_export2.h"
//#include "irisengine_export_zbio.h"


#ifdef __cplusplus
extern "C" {
#endif

#define STRLEN 4096

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineInit(JNIEnv *env, jobject thiz, jstring conf);

JNIEXPORT void JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineRelease(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineRelease2(JNIEnv *env, jobject thiz, jboolean release_data);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineEnroll(JNIEnv *env, jobject thi, jstring jstrName,
                                                      jint cover);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineIdentify(JNIEnv *env, jobject thi, jstring jstrName,
                                                        int continuous);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineStop(JNIEnv *env, jobject thi);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEnginePauseAction(JNIEnv *env, jobject thi);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineResumeAction(JNIEnv *env, jobject thi);

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetUserList(JNIEnv *env, jobject thi);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineDeleteUser(JNIEnv *env, jobject thi, jstring user);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineDeleteAllUser(JNIEnv *env, jobject thiz);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineSetPreview(JNIEnv *env, jobject thi, jstring mode,
                                                          jint format, jint width, jint height,
                                                          jint marginX, jint marginY);


JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineStartPurePreview(JNIEnv *env, jobject thi);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineStopPurePreview(JNIEnv *env, jobject thi);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineChangeConfigure(JNIEnv *env, jobject thi,
                                                               jstring config);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineLoadDevParams(JNIEnv *env, jobject thi,
                                                             jstring config);

JNIEXPORT int JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineAddIrisUser(JNIEnv *env, jobject thi, jstring uid,
                                                           jbyteArray left_iris,
                                                           jbyteArray right_iris);

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetDeviceInfo2(JNIEnv *env, jobject thi);

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetEngineInfo(JNIEnv *env, jobject thi);

JNIEXPORT jbyteArray JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetIrisCode(JNIEnv *env,
                                                           jobject thiz,
                                                           jbyteArray frame,
                                                           jint size,
                                                           jstring format);

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetEnrollData(JNIEnv *env, jobject thiz, jstring uid);

JNIEXPORT jstring JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineGetIdentifyData(JNIEnv *env, jobject thiz);

JNIEXPORT jint JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineUpdateFirmware(JNIEnv *env, jobject thiz,
                                                              jbyteArray firmware);

JNIEXPORT jint JNICALL
Java_cn_simbok_irisHelper_IrisHelper_IrisEngineCheckHardware(JNIEnv *env, jobject thiz);


#ifdef __cplusplus
};
#endif

#endif //IRISENGINEDEMO_IrisHelper_H
