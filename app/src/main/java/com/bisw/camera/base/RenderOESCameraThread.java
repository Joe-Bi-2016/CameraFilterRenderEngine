package com.bisw.camera.base;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.graphics.SurfaceTexture;
import android.view.Surface;
import android.view.SurfaceHolder;
import com.bisw.filterRenderEngine.NativeGLRender;

public class RenderOESCameraThread extends Thread{
    private static final String TAG = RenderOESCameraThread.class.getSimpleName();
    private Handler mMainHandler;
    private RenderOESCameraHandler mHandler;
    private AssetManager mAssetManager;
    private Object mStartLock = new Object();
    private boolean mReady = false;
    private int mTextureId;
    private SurfaceTexture mSurfaceTexture;
    public static final int MSG_SURFACE_AVAILABLE = 0;
    private NativeGLRender mNativeEglRender;
    private String mTextureMatName;

    public RenderOESCameraThread(Context context, Handler handler) {
        mMainHandler = handler;
        mAssetManager = context.getAssets();
    }

    public RenderOESCameraHandler getRenderHandler() {
        return mHandler;
    }

    /**
     *
     */
    @Override
    public void run() {
        super.run();
        Log.i(TAG, "Render thread start running");

        Looper.prepare();

        mHandler = new RenderOESCameraHandler(this);
        try {
            mNativeEglRender = new NativeGLRender();
        }catch (Exception e) {
            Log.e(TAG, e.getMessage());
        }

        synchronized (mStartLock) {
            mReady = true;
            mStartLock.notify();
        }

        Looper.loop();
        releaseGL();
        mHandler = null;
        mMainHandler = null;
        Log.i(TAG, "Render thread exit");
    }

    public void waitReady() {
        synchronized (mStartLock) {
            while (!mReady) {
                try {
                    mStartLock.wait();
                } catch (InterruptedException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }
    }

    public void shutdown() {
        Looper.myLooper().quit();
    }

    // create EGL operating environment
    public void onSurfaceAvailable(Object surface) {
        if (surface instanceof SurfaceTexture) {
            mNativeEglRender.native_initWithSurface(new Surface((SurfaceTexture) surface));
        } else if (surface instanceof SurfaceHolder) {
            mNativeEglRender.native_initWithSurface(((SurfaceHolder) surface).getSurface());
        }

        initGL();

        // create surfacetexture
        mSurfaceTexture = new SurfaceTexture(mTextureId);
    }

    public void surfaceChanged(int width, int height) {
        mNativeEglRender.native_onSurfaceChanged(width, height);
    }

    public void surfaceDestroyed() {
        mNativeEglRender.native_onDestroySurface();
    }

    public void frameAvailable() {
        onDrawFrame();
    }

    public void setCameraPreviewSize(int width, int height) {
        mNativeEglRender.native_onSurfaceChanged(width, height);
    }

    public void setShaderName(String programName, String vert, String frag) {
        mNativeEglRender.native_setProgramShaderFromAssets(0, programName, vert, frag);
    }

    public void setShaderString(String programName, String vert, String frag) {
        mNativeEglRender.native_setProgramShader(0, programName, vert, frag);
    }

    // create GL variables
    private void initGL() {
        mNativeEglRender.native_setAssetsManger(mAssetManager);
        mNativeEglRender.native_setBackgroundColor(0.0f, 1.0f, 0.0f, 1.0f);
        mTextureId = mNativeEglRender.native_createOESTexture(0, "oesTextureId");
    }

    public void startRender() {
        // Pass the surface to the main thread for further processing by the main thread
        Message msg = new Message();
        msg.what = MSG_SURFACE_AVAILABLE; msg.obj = mSurfaceTexture;
        mMainHandler.sendMessage(msg);
    }

    public void setUniform(String name, float[] matrix) {
        mNativeEglRender.native_setParamMatrix(0, name, matrix);
    }

    public void setTextureMatrixName(String name) {
        mTextureMatName = name;
    }

    // gl draw
    private void onDrawFrame() {
        mSurfaceTexture.updateTexImage();
        if (mTextureMatName != null) {
            // get matrix of texture
            float[] textureMatrix = new float[16];
            mSurfaceTexture.getTransformMatrix(textureMatrix);
            mNativeEglRender.native_setParamMatrix(0, mTextureMatName, textureMatrix);
        }
        mNativeEglRender.native_onDrawFrame(0);
    }

    // release GL
    private void releaseGL() {
        mNativeEglRender.native_release();
    }
}
