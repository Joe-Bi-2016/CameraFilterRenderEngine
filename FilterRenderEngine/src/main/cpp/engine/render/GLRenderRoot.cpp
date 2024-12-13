//
// Created by Joe.Bi on 2024/11/25.
//

#include <stdint.h>
#include "GLRenderRoot.h"
#include "GLProgramManager.h"
#include "LogUtil.h"
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string>
#include <typeinfo>

//----------------------------------------------------------------------------------------//
BEGIN

    //------------------------------------------------------------------------------------//
    typedef void* (*threadFunc)(void*);

    map<pthread_t, GLRenderThreadBase*> GLRenderRoot::mBackThread = map<pthread_t, GLRenderThreadBase*>();

    //------------------------------------------------------------------------------------//
    static int createRawThread(threadFunc entryFunction,
                               void *userData,
                               const char *threadName,
                               int32_t threadPriority,
                               size_t threadStackSize,
                               pthread_t *threadId)
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (threadStackSize)
            pthread_attr_setstacksize(&attr, threadStackSize);

        pthread_t thread;
        int result = pthread_create(&thread, &attr, entryFunction, userData);
        pthread_attr_destroy(&attr);
        if (result != 0)
        {
            LOGERROR("CreateRawThreadEtc failed (entry=%p, res=%d)\n (android threadPriority=%d)", entryFunction, result, threadPriority);
            return 0;
        }

        if (threadId != NULL)
        {
            *threadId = thread; // XXX: this is not portable
        }

        return 1;
    }

    //------------------------------------------------------------------------------------//
    void* GLRenderRoot::threadLoop(void *pParam)
    {
        assert(pParam);
        threadParam* param = (threadParam*)pParam;
        assert(param->objcreator);
        param->renderbaseptr = param->objcreator(nullptr);
        param->renderbaseptr->initWithEglCore(param->eglcore);
        pthread_t threadId = syscall(SYS_gettid);
        mBackThread.insert(mapVal(threadId, param->renderbaseptr));

        for(;;)
        {
            if(param->renderbaseptr->isShouldExit())
                break;
            param->renderbaseptr->onDo();
        }

        return 0;
    }

    //------------------------------------------------------------------------------------//
    GLRenderRoot* GLRenderRoot::mRenderRoot = nullptr;
    std::mutex GLRenderRoot::mMutex;

    //------------------------------------------------------------------------------------//
    GLRenderRoot::GLRenderRoot(void)
    : mMainThreadId(0)
    , mMainThreadRoot(nullptr)
	, mDefaultFbo(nullptr)
    {
        mMainThreadId = syscall(SYS_gettid);
    }

    //------------------------------------------------------------------------------------//
    GLRenderRoot::~GLRenderRoot(void)
    {
        mDefaultFbo.reset();
        safeDeleteObject(mMainThreadRoot);
        mMainThreadId = 0;
    }

    //------------------------------------------------------------------------------------//
    void GLRenderRoot::initWithEglCore(void)
    {
        mMainThreadRoot = safeNewObject(GLRenderThreadMain);
        mMainThreadRoot->initWithEglCore(nullptr);
        mDefaultFbo = GLFrameBuffer::createFbo();
    }

    //------------------------------------------------------------------------------------//
    void GLRenderRoot::initWithWindow(ANativeWindow *nativeWindow)
    {
        mMainThreadRoot = safeNewObject(GLRenderThreadMain);
        mMainThreadRoot->initWithWindow(nativeWindow);
        mDefaultFbo = GLFrameBuffer::createFbo();
    }

    //------------------------------------------------------------------------------------//
    GLRenderRoot* GLRenderRoot::getInstance(void)
    {
        if(mRenderRoot == nullptr)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if(mRenderRoot == nullptr)
                mRenderRoot = new GLRenderRoot();
        }

        return mRenderRoot;
    }

    //------------------------------------------------------------------------------------//
    void GLRenderRoot::release(void)
    {
        if(!mBackThread.empty())
        {
            mapIt it = mBackThread.begin();
            for(; it != mBackThread.end(); )
            {
                // should send a message to thread that itself exit
                it->second->setMessage(nullptr);
                pthread_join(it->first, nullptr);
                safeDeleteObject(it->second);
                it = mBackThread.erase(it);
            }

            mBackThread.clear();
        }

        if (mRenderRoot)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            safeDeleteObject(mRenderRoot);
        }

        // global static variables should been release first
        GLQuadModel::releaseQuadModel();

        GLProgramManager::releaseGLProgramManager();
    }

    //------------------------------------------------------------------------------------//
    GLRenderThreadBase* GLRenderRoot::forkThread(int width, int height, const createRenderThreadObject& objcreator)
    {
        threadParam param;
        param.eglcore = mMainThreadRoot->getEGLContext();
        param.objcreator = objcreator;
        param.renderbaseptr = nullptr;

        pthread_t threadId;
        createRawThread(threadLoop, &param, nullptr, 0, 0, &threadId);
        do {
            if(param.renderbaseptr == nullptr)
                sleep(10);
        }while (param.renderbaseptr == nullptr);

        return param.renderbaseptr;
    }

    //------------------------------------------------------------------------------------//
    bool GLRenderRoot::releaseThread(GLRenderThreadBase* thread)
    {
        if(!mBackThread.empty())
        {
            map<pthread_t, GLRenderThreadBase*>::iterator it = mBackThread.begin();
            for (; it != mBackThread.end(); )
            {
                if((*it).second == thread)
                {
                    // set message to it thread exiting
                    it->second->setMessage(nullptr);
                    pthread_join(it->first, nullptr);
                    mBackThread.erase(it);
                    return true;
                }
            }

            LOGINFO("does not find the thread from background thread list");
            return false;
        }
        else
        {
            LOGINFO("the back ground thread list is null");
            return false;
        }
    }

    //------------------------------------------------------------------------------------//
    void GLRenderRoot::setbackgroundColor(const float r, float g, float b, float a)
    {
        if (mDefaultFbo)
            mDefaultFbo->setClearColor(r, g, b, a);
        mMainThreadRoot->setBackgroundColor(r, g, b, a);
    }

    //------------------------------------------------------------------------------------//
    void GLRenderRoot::onSurfaceChanged(int width, int height)
    {
        if(mMainThreadRoot)
        {
            mMainThreadRoot->resizeView(width, height);

            bool offscreen = mMainThreadRoot->getEGLContext()->getDrawEGLSurface()->isOffScreen();
            if(offscreen && mDefaultFbo)
            {
                if(width != 0 && height != 0 && (mDefaultFbo->getWidth() != width || mDefaultFbo->getHeight() != height))
                {
                    mDefaultFbo->init(width, height);
                }
            }
        }
    }

    //------------------------------------------------------------------------------------//
    void GLRenderRoot::drawOneFrame(int renderPassIndex/* = -1 */)
    {
        LOGINFO("draw one frame start......");

        if (mDefaultFbo)
        {
            mDefaultFbo->clearFBO();
            mDefaultFbo->useFBO();
        }

        // send message to all background threads to be deal with
        if (!mBackThread.empty())
        {
            map<pthread_t, GLRenderThreadBase*>::iterator it = mBackThread.begin();
            for (; it != mBackThread.end(); )
            {
                it->second->setMessage(nullptr);
            }
        }

        mMainThreadRoot->onDo(renderPassIndex);

        if(mDefaultFbo && mDefaultFbo.get())
        {
            // If fbo uses EGL textures, then glFinish() should be called, otherwise there is no data output to EGLImage
            sharedRenderTarget target = mDefaultFbo->getRenderTarget();
            if (target && typeid(*(target->getTexture())) == typeid(GLEGLImageTexture))
                glFinish();
        }

        mMainThreadRoot->getEGLContext()->swapBuffers();

        LOGINFO("draw end......");
    }

END

