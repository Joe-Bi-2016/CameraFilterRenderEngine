//
// Created by Joe.Bi on 2024/11/23.
//

#ifndef __JNIGLEngine_h__
#define __JNIGLEngine_h__
#include <jni.h>

#ifdef __cplusplus
    extern "C" {
#endif

JNIEXPORT void JNICALL initWithWindow(JNIEnv *env, jobject instance, jobject surface);

JNIEXPORT void JNICALL init(JNIEnv *env, jobject instance);

JNIEXPORT void JNICALL destroySurface(JNIEnv *env, jobject instance);

JNIEXPORT void JNICALL release(JNIEnv *env, jobject instance);

JNIEXPORT void JNICALL setProgramShader(JNIEnv *env, jobject instance, jint renderPassIndex, jstring programName, jstring vertShader, jstring fragShader);

JNIEXPORT void JNICALL setAssetsManger(JNIEnv *env, jobject instance, jobject assetManager);

JNIEXPORT void JNICALL setProgramShaderFromAssets(JNIEnv *env, jobject instance, jint renderPassIndex, jstring programName, jstring vertShader, jstring fragShader);

JNIEXPORT void JNICALL setTexture(JNIEnv *env, jobject instance, jint format, jint width, jint height, jbyteArray imageData, jint renderPassIndex, jint index);

JNIEXPORT int JNICALL createOESTexture(JNIEnv *env, jobject instance, jint renderPassIndex, jstring name);

JNIEXPORT void JNICALL setBackgroundColor(jfloat r, jfloat g, jfloat b, jfloat a);

JNIEXPORT void JNICALL addRenderPass(jint renderPassIndex); // renderPassIndex from 1 start

JNIEXPORT void JNICALL setRenderPassBackGroundColor(jint renderPassIndex, jfloat r, jfloat g, jfloat b, jfloat a);

JNIEXPORT void JNICALL addRenderFboToRenderPass(jint renderPassIndex, jint fboRenderPassIndex);

JNIEXPORT void JNICALL setParamInt(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jint value);

JNIEXPORT void JNICALL setParamFloat(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jfloat value);

JNIEXPORT void JNICALL setParamBool(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jboolean value);

JNIEXPORT void JNICALL setParamVector(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jfloatArray value);

JNIEXPORT void JNICALL setParamMatrix(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jfloatArray value);

JNIEXPORT void JNICALL onSurfaceCreated(JNIEnv *env, jobject instance);

JNIEXPORT void JNICALL onSurfaceChanged(JNIEnv *env, jobject instance, jint width, jint height);

JNIEXPORT void JNICALL onDrawFrame(JNIEnv *env, jobject instance, jint renderPassIndex);

JNIEXPORT jobject JNICALL readPixel(JNIEnv *env, jobject instance, int x, int y, int width, int height);

JNIEXPORT jbyteArray JNICALL getPixel(JNIEnv *env, jobject instance, int x, int y, int width, int height);

#ifdef __cplusplus
    }
#endif



#endif //__JNIGLEngine_h__

