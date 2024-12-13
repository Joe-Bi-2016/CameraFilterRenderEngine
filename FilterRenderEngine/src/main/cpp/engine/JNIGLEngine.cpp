//
// Created by Joe.Bi on 2024/11/23.
//

#include "JNIGLEngine.h"
#include "util/LogUtil.h"
#include "render/GLRenderRoot.h"
#include "render/GLQuadRenderPass.h"
#include "render/GLDefaultShader.h"
#include "JNIAssetFile.h"
#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>
#include <iostream>
#include <android/bitmap.h>

using namespace filterRenderEngine;

#define NATIVE_RENDER_CLASS_NAME "com/bisw/filterRenderEngine/NativeGLRender"

//-----------------------------------------------------------------------------------------//
JNIEXPORT std::string JNICALL jstring2String(JNIEnv *env, jstring jStr)
{
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes, env->NewStringUTF("UTF-8"));

    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte* pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char *)pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}

#ifdef __cplusplus
    extern "C" {
#endif

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL initWithWindow(JNIEnv *env, jobject instance, jobject surface)
{
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    GLRenderRoot::getInstance()->initWithWindow(window);

    sharedRenderPass renderPass = std::make_shared<GLQuadRenderPass>();
    GLRenderRoot::getInstance()->getMainThreadRoot()->attacheRenderPass(renderPass);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL init(JNIEnv *env, jobject instance)
{
    GLRenderRoot::getInstance()->initWithEglCore();

    sharedRenderPass renderPass = std::make_shared<GLQuadRenderPass>();
    GLRenderRoot::getInstance()->getMainThreadRoot()->attacheRenderPass(renderPass);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL destroySurface(JNIEnv *env, jobject instance)
{
    if (GLRenderRoot::getInstance())
    {
        if (GLRenderRoot::getInstance()->getMainThreadRoot())
            GLRenderRoot::getInstance()->getMainThreadRoot()->destroyEGLSurface();
    }
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL release(JNIEnv *env, jobject instance)
{
    GLRenderRoot::release();
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setProgramShader(JNIEnv *env, jobject instance, jint renderPassIndex, jstring programName, jstring vertShader, jstring fragShader)
{
    string name = jstring2String(env, programName);
    string vert = jstring2String(env, vertShader);
    string frag = jstring2String(env, fragShader);
    GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->setProgram(name.c_str(), vert.c_str(), frag.c_str());
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setAssetsManger(JNIEnv *env, jobject instance, jobject assetManager)
{
    AAssetManager *nativeasset = AAssetManager_fromJava(env, assetManager);
    setAssetManager((void*)nativeasset);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setProgramShaderFromAssets(JNIEnv *env, jobject instance, jint renderPassIndex, jstring programName, jstring vertShader, jstring fragShader)
{
    string name = jstring2String(env, programName);
    string vert = jstring2String(env, vertShader);
    string frag = jstring2String(env, fragShader);
    uint64_t vertShaderLen = 0, fragShaderLen = 0;
    FILE* vertFile = fileOpen(vert.c_str(), (const char*)"r", &vertShaderLen);
    FILE* fragFile = fileOpen(frag.c_str(), (const char*)"r", &fragShaderLen);
    char* vertStr = new char[vertShaderLen + 1];
    char* fragStr = new char[fragShaderLen  +1];
    memset(vertStr, 0x0, (vertShaderLen + 1));
    memset(fragStr, 0x0, (fragShaderLen + 1));
    fileRead(vertFile, nullptr, vertStr, vertShaderLen, 1);
    fileRead(fragFile, nullptr, fragStr, fragShaderLen, 1);
    fileClose(vertFile);
    fileClose(fragFile);

    GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->setProgram(name.c_str(), vertStr, fragStr);

    delete [] vertStr; vertStr = nullptr;
    delete [] fragStr; fragStr = nullptr;
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setTexture(JNIEnv *env, jobject instance, jint format, jint width, jint height, jbyteArray imageData, jint renderPassIndex, jint index)
{
    int len = env->GetArrayLength (imageData);
    if(len == 0)
    {
        LOGERROR("image data is null!");
        return;
    }

    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));

    sharedTexture texture = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getTexture(index);
    if(texture.get() == nullptr)
    {
        GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->addTexture(GLTEXTURE);
        texture = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getTexture(index);
    }

    if(texture->textureDataIsNull())
        texture->initWithData(width, height, PixelFormat(format), buf, len, width, height);
    else
        texture->updateTexture(buf, len, PixelFormat(format), 0, 0, width, height);

    delete [] buf;
    env->DeleteLocalRef(imageData);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT int JNICALL createOESTexture(JNIEnv *env, jobject instance, jint renderPassIndex, jstring name)
{
    string txtName = jstring2String(env, name);
    sharedTexture texture = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getTexture(txtName.c_str());
    if(texture.get() == nullptr)
    {
        GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->addTexture(OESTEXTURE, txtName.c_str());
        texture = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getTexture(txtName.c_str());
    }

    return texture->getTextureId();
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setBackgroundColor(jfloat r, jfloat g, jfloat b, jfloat a)
{
    GLRenderRoot::getInstance()->setbackgroundColor(r, g, b, a);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL addRenderPass(jint renderPassIndex)
{
    if(GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex) == nullptr)
    {
        sharedRenderPass renderPass = std::make_shared<GLQuadRenderPass>();
        GLRenderRoot::getInstance()->getMainThreadRoot()->attacheRenderPass(renderPass);
    }
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setRenderPassBackGroundColor(jint renderPassIndex, jfloat r, jfloat g, jfloat b, jfloat a)
{
    sharedRenderPass renderPass = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex);
    if(renderPass.get())
        renderPass->setClearColor(r, g, b, a);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL addRenderFboToRenderPass(jint renderPassIndex, jint fboRenderPassIndex)
{
    sharedRenderPass renderPass = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex);
    sharedRenderPass fborenderPass = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(fboRenderPassIndex);
    if(renderPass.get() && fborenderPass.get())
        renderPass->addTexture(fborenderPass->getFboColorBuffer());
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setParamInt(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jint value)
{
    sharedProgram program = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getProgram();
    string name = jstring2String(env, paramName);
    program->setUniform(name, &value, 1);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setParamFloat(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jfloat value)
{
    sharedProgram program = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getProgram();
    string name = jstring2String(env, paramName);
    program->setUniform(name, &value, 1);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setParamBool(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jboolean value)
{
    sharedProgram program = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getProgram();
    string name = jstring2String(env, paramName);
    bool ret = value == JNI_TRUE;
    program->setUniform(name, &ret, 1);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setParamVector(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jfloatArray value)
{
    sharedProgram program = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getProgram();
    string name = jstring2String(env, paramName);
    size_t length = (size_t)env->GetArrayLength(value);
    float* data = (float*)env->GetFloatArrayElements(value, 0);
    switch(length)
    {
        case 2:
            program->setUniform(name, (glm::vec2*)data, 1);
            break;
        case 3:
            program->setUniform(name, (glm::vec3*)data, 1);
            break;
        case 4:
            program->setUniform(name, (glm::vec4*)data, 1);
            break;
    }

    env->ReleaseFloatArrayElements(value, data, JNI_ABORT);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL setParamMatrix(JNIEnv *env, jobject instance, jint renderPassIndex, jstring paramName, jfloatArray value)
{
    sharedProgram program = GLRenderRoot::getInstance()->getMainThreadRoot()->getRenderPass(renderPassIndex)->getProgram();
    string name = jstring2String(env, paramName);
    size_t length = (size_t)env->GetArrayLength(value);
    float* data = (float*)env->GetFloatArrayElements(value, 0);
    switch(length)
    {
        case 4: // mat2
            program->setUniform(name, (glm::mat2*)data, 1);
            break;
        case 9: // mat3
            program->setUniform(name, (glm::mat3*)data, 1);
            break;
        case 16: // mat4
            program->setUniform(name, (glm::mat4*)data, 1);
            break;
    }

    env->ReleaseFloatArrayElements(value, data, JNI_ABORT);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL onSurfaceCreated(JNIEnv *env, jobject instance)
{
    GLRenderRoot::getInstance()->setbackgroundColor(1.0f, 1.0f, 1.0f, 1.0f);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL onSurfaceChanged(JNIEnv *env, jobject instance, jint width, jint height)
{
    GLRenderRoot::getInstance()->onSurfaceChanged(width, height);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT void JNICALL onDrawFrame(JNIEnv *env, jobject instance, jint renderPassIndex)
{
    GLRenderRoot::getInstance()->drawOneFrame(renderPassIndex);
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT jobject JNICALL readPixel(JNIEnv *env, jobject instance, int x, int y, int width, int height)
{
    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass, valueOfBitmapConfigFunction, configName);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, width, height, bitmapConfig);

    // free memory and reference
    LOGBVERBORSE("free memory and reference");
    env->DeleteLocalRef(bitmapCls);
    env->DeleteLocalRef(bitmapConfigClass);
    env->DeleteLocalRef(configName);

    void* bitmapPixels;
    int ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels);
    if (ret < 0 || bitmapPixels == NULL)
    {
        LOGERROR("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return NULL;
    }

    GLRenderRoot::getInstance()->getDefaultFbo()->getPixels(x, y, width, height, RGBA8888, (uint8_t*)bitmapPixels);

    AndroidBitmap_unlockPixels(env, newBitmap);

    return newBitmap;
}

//-----------------------------------------------------------------------------------------//
JNIEXPORT jbyteArray JNICALL getPixel(JNIEnv *env, jobject instance, int x, int y, int width, int height)
{
    jbyteArray ret = env->NewByteArray(width * height * 4);
    uint8_t* bitmapPixels = (uint8_t*)env->GetByteArrayElements(ret, 0);
    GLRenderRoot::getInstance()->getDefaultFbo()->getPixels(x, y, width, height, RGBA8888, bitmapPixels);
    return ret;
}

#ifdef __cplusplus
    }
#endif

//-----------------------------------------------------------------------------------------//
static JNINativeMethod MethodLists[] =
{
    {"native_initWithSurface",              "(Landroid/view/Surface;)V",                                    (void *)(initWithWindow)},
    {"native_init",                         "()V",                                                          (void *)(init)},
    {"native_onDestroySurface",             "()V",                                                          (void *)(destroySurface)},
    {"native_release",                      "()V",                                                          (void *)(release)},
    {"native_setProgramShader",             "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",   (void *)(setProgramShader)},
    {"native_setAssetsManger",              "(Landroid/content/res/AssetManager;)V",                        (void *)(setAssetsManger)},
    {"native_setProgramShaderFromAssets",   "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",   (void *)(setProgramShaderFromAssets)},
    {"native_setTexture",                   "(III[BII)V",                                                   (void *)(setTexture)},
    {"native_createOESTexture",             "(ILjava/lang/String;)I",                                       (void *)(createOESTexture)},
    {"native_setBackgroundColor",           "(FFFF)V",                                                      (void *)(setBackgroundColor)},
    {"native_addRenderPass",                "(I)V",                                                         (void *)(addRenderPass)},
    {"native_setRenderPassBackGroundColor", "(IFFFF)V",                                                     (void *)(setRenderPassBackGroundColor)},
    {"native_addRenderFboToRenderPass",     "(II)V",                                                        (void *)(addRenderFboToRenderPass)},
    {"native_setParamInt",                  "(ILjava/lang/String;I)V",                                      (void *)(setParamInt)},
    {"native_setParamFloat",                "(ILjava/lang/String;F)V",                                      (void *)(setParamFloat)},
    {"native_setParamBool",                 "(ILjava/lang/String;Z)V",                                      (void *)(setParamBool)},
    {"native_setParamVector",               "(ILjava/lang/String;[F)V",                                     (void *)(setParamVector)},
    {"native_setParamMatrix",               "(ILjava/lang/String;[F)V",                                     (void *)(setParamMatrix)},
    {"native_onSurfaceCreated",             "()V",                                                          (void *)(onSurfaceCreated)},
    {"native_onSurfaceChanged",             "(II)V",                                                        (void *)(onSurfaceChanged)},
    {"native_onDrawFrame",                  "(I)V",                                                         (void *)(onDrawFrame)},
    {"native_readPixel",                    "(IIII)Landroid/graphics/Bitmap;",                              (void *)(readPixel)},
    {"native_getPixel",                     "(IIII)[B",                                                     (void *)(getPixel)},
};

//-----------------------------------------------------------------------------------------//
static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum)
{
    LOGBVERBORSE("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if(clazz == NULL)
    {
        LOGBVERBORSE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0)
    {
        LOGBVERBORSE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

//-----------------------------------------------------------------------------------------//
static void unregisterNativeMethods(JNIEnv *env, const char *className)
{
    LOGBVERBORSE("UnregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL)
    {
        LOGBVERBORSE("UnregisterNativeMethods fail. clazz == NULL");
        return;
    }
    if (env != NULL)
    {
        env->UnregisterNatives(clazz);
    }
}

//-----------------------------------------------------------------------------------------//
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p)
{
    LOGBVERBORSE("-------- JNI_OnLoad ---------");
    jint ret = JNI_ERR;
    JNIEnv* env = nullptr;
    if(jvm->GetEnv((void**)(&env), JNI_VERSION_1_6) != JNI_OK)
        return ret;

    jint regret = registerNativeMethods(env, NATIVE_RENDER_CLASS_NAME, MethodLists, sizeof(MethodLists) / sizeof(MethodLists[0]));
    if(regret != JNI_TRUE)
        return JNI_ERR;

    return JNI_VERSION_1_6;
}

//-----------------------------------------------------------------------------------------//
extern "C" void JNI_OnUnload(JavaVM *jvm, void *p)
{
    JNIEnv* env = nullptr;
    if(jvm->GetEnv((void**)(&env), JNI_VERSION_1_6) != JNI_OK)
        return;

    unregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);
}