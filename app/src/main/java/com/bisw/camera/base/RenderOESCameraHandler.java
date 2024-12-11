package com.bisw.camera.base;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import java.lang.ref.WeakReference;

public class RenderOESCameraHandler extends Handler {
    private static final String TAG = RenderOESCameraHandler.class.getSimpleName();
    private static final int MSG_SURFACE_AVAILABLE = 0;
    private static final int MSG_SURFACE_CHANGED = 1;
    private static final int MSG_SURFACE_DESTROYED = 2;
    private static final int MSG_SHUTDOWN = 3;
    private static final int MSG_FRAME_AVAILABLE = 4;
    private static final int MSG_SIZE_VALUE = 5;
    private static final int MSG_SHADERNAME_VALUE = 6;
    private static final int MSG_SHADER_VALUE = 7;
    private static final  int MSG_START_RENDER = 8;
    private static final int MSG_SET_UNIFORM = 9;
    private static final int MSG_SET_TXTUREUNIFORMNAME = 10;
    private WeakReference<Thread> mWeakRenderThread;

    public RenderOESCameraHandler(Thread thread) {
        mWeakRenderThread = new WeakReference<>(thread);
    }

    public void sendSurfaceAvailable(Object surface) {
        sendMessage(obtainMessage(MSG_SURFACE_AVAILABLE, 0, 0, surface));
    }

    public void sendSurfaceChanged(int width, int height) {
        sendMessage(obtainMessage(MSG_SURFACE_CHANGED, width, height));
    }

    public void sendSurfaceDestroyed() {
        sendMessage(obtainMessage(MSG_SURFACE_DESTROYED));
    }

    public void sendShutdown() {
        sendMessage(obtainMessage(MSG_SHUTDOWN));
    }

    public void sendFrameAvailable() {
        sendMessage(obtainMessage(MSG_FRAME_AVAILABLE));
    }

    public void sendShaderName(String programName, String vert, String frag) {
        String str = programName + "|" + vert + "|" + frag;
        sendMessage(obtainMessage(MSG_SHADERNAME_VALUE, 0, 0, str));
    }

    public void setShaderString(String programName, String vert, String frag) {
        String str = programName + "|" + vert + "|" + frag;
        sendMessage(obtainMessage(MSG_SHADER_VALUE, 0, 0, str));
    }

    public void sendPreviewSize(int width, int height) {
        sendMessage(obtainMessage(MSG_SIZE_VALUE, width, height));
    }

    public void sendStartRender() {
        sendMessage(obtainMessage(MSG_START_RENDER));
    }

    public void sendUniform(String uniformName, float[] matrix) {
        Bundle bundle = new Bundle();
        bundle.putString("name", uniformName);
        bundle.putFloatArray("value", matrix);
        Message msg = obtainMessage(MSG_SET_UNIFORM);
        msg.setData(bundle);
        sendMessage(msg);
    }

    public void setTextureMatrixName(String uniformName) {
        sendMessage(obtainMessage(MSG_SET_TXTUREUNIFORMNAME, 0, 0, uniformName));
    }

    @Override
    public void handleMessage(Message msg) {
        RenderOESCameraThread renderThread = (RenderOESCameraThread) mWeakRenderThread.get();
        if (renderThread == null) {
            Log.w(TAG, "RenderHandler.handleMessage: weak ref is null");
            return;
        }
        switch (msg.what) {
            case MSG_SURFACE_AVAILABLE:
                renderThread.onSurfaceAvailable(msg.obj);
                break;
            case MSG_SURFACE_CHANGED:
                renderThread.surfaceChanged(msg.arg1, msg.arg2);
                break;
            case MSG_SURFACE_DESTROYED:
                renderThread.surfaceDestroyed();
                break;
            case MSG_SHUTDOWN:
                renderThread.shutdown();
                break;
            case MSG_FRAME_AVAILABLE:
                renderThread.frameAvailable();
                break;
            case MSG_SIZE_VALUE:
                renderThread.setCameraPreviewSize(msg.arg1, msg.arg2);
                break;
            case MSG_SHADERNAME_VALUE:
            {
                String[] strs = ((String)msg.obj).split("\\|");
                renderThread.setShaderName(strs[0], strs[1], strs[2]);
            }
                break;
            case MSG_SHADER_VALUE:
                String[] strs = ((String)msg.obj).split("\\|");
                renderThread.setShaderString(strs[0], strs[1], strs[2]);
            break;
            case MSG_START_RENDER:
                renderThread.startRender();
                break;
            case MSG_SET_UNIFORM:
            {
                Bundle b = msg.getData();
                String name = b.getString("name");
                float[] val = b.getFloatArray("value");
                renderThread.setUniform(name, val);
            }
                break;
            case MSG_SET_TXTUREUNIFORMNAME:
                renderThread.setTextureMatrixName((String) msg.obj);
                break;
            default:
                throw new RuntimeException("unknown message " + msg.what);
        }
    }

}
