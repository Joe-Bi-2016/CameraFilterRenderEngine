package com.bisw.camera.base;

import android.content.Context;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import androidx.annotation.NonNull;
import static android.opengl.Matrix.setIdentityM;
import static android.opengl.Matrix.setRotateM;
import static android.opengl.Matrix.multiplyMM;

public abstract class BaseGLESCameraSurfaceView extends SurfaceView
        implements SurfaceHolder.Callback,
        BaseView,
        CameraCallback{
    private static final String TAG = BaseGLESCameraTextureView.class.getSimpleName();
    private SurfaceHolder mSurfaceHolder;
    private Handler mMainHandler;
    private RenderOESCameraThread mRenderThread;
    private SurfaceTexture mSurfaceTexture;
    private int mWidth;
    private int mHeight;
    private ICameraManager mCameraManager;
    protected String mVertName, mFragName;
    protected String mVertStr, mFragStr;
    protected String mProgramName;
    protected boolean mhadSetProgram;
    protected String mEGLModelMartrixName, mEGLTextureMatrixName;

    public BaseGLESCameraSurfaceView(Context context) {
        this(context, null);
    }

    public BaseGLESCameraSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public BaseGLESCameraSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public BaseGLESCameraSurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mSurfaceHolder = getHolder();
        mSurfaceHolder.setFormat(PixelFormat.TRANSPARENT);
        mSurfaceHolder.addCallback(this);

        mMainHandler = new Handler(Looper.myLooper()) {
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);
                switch (msg.what) {
                    case RenderOESCameraThread.MSG_SURFACE_AVAILABLE:
                        onGLSurfaceCreated((SurfaceTexture) msg.obj);
                        break;
                }
            }
        };

        mRenderThread = new RenderOESCameraThread(context, mMainHandler);
        mRenderThread.start();
        mRenderThread.waitReady();

        mCameraManager = createCameraManager(context);
        mCameraManager.setCameraCallback(this);
        mWidth = mHeight = 0;

        mVertName = mFragName = null;
        mVertStr = mFragStr = null;

        mhadSetProgram = false;
    }

    /**
     * @param surfaceHolder
     */
    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        if (surfaceHolder != null) {
            Rect rect = surfaceHolder.getSurfaceFrame();
            mWidth = rect.width();
            mHeight = rect.height();

            if(mRenderThread != null) {
                RenderOESCameraHandler handler = mRenderThread.getRenderHandler();
                if (handler != null) {
                    handler.sendSurfaceAvailable(surfaceHolder);
                    handler.sendSurfaceChanged(mWidth, mHeight);
                    handler.sendStartRender();
                }
            }
        }
    }

    /**
     * @param surfaceHolder
     * @param i
     * @param width
     * @param height
     */
    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int width, int height) {
        mWidth = width;
        mHeight = height;
        if(mRenderThread != null) {
            RenderOESCameraHandler handler = mRenderThread.getRenderHandler();
            if (handler != null) {
                handler.sendSurfaceChanged(mWidth, mHeight);
            }
        }
    }

    /**
     * @param surfaceHolder
     */
    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        Log.v(TAG, "surfaceDestroyed.");
        // set message to render thread to exit
        if(mRenderThread != null) {
            RenderOESCameraHandler handler = mRenderThread.getRenderHandler();
            if (handler != null) {
                handler.sendSurfaceDestroyed();
            }
        }
        // camera exit
        mSurfaceTexture = null;
        mCameraManager.releaseCamera();
    }

    /**
     * @param widthMeasureSpec
     * @param heightMeasureSpec
     */
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);
        if (0 == mWidth || 0 == mHeight) {
            setMeasuredDimension(width, width * 4 / 3);
        } else {
            if (width > height * mWidth / mHeight) {
                setMeasuredDimension(width, width * mHeight / mWidth);
            } else {
                setMeasuredDimension(height * mWidth / mHeight, height);
            }
        }
    }

    /**
     *
     */
    @Override
    public void Pause() {
        mCameraManager.releaseCamera();
    }

    /**
     *
     */
    @Override
    public void Resume() {
        if(mWidth > 0 && mHeight > 0)
            openCamera();
    }

    /**
     *
     */
    @Override
    public void Close() {
        mCameraManager.releaseCamera();
        if (mRenderThread != null) {
            RenderOESCameraHandler handler = mRenderThread.getRenderHandler();
            if (handler != null) {
                handler.sendShutdown();
            }
        }
    }

    /**
     * @return
     */
    @Override
    public ICameraManager getCameraManager() {
        return mCameraManager;
    }

    public abstract ICameraManager createCameraManager(Context context);
    public abstract void setShaderName(String programName, String vert, String frag);
    public abstract void setShaderString(String programName, String vert, String frag);

    public void setEGLMatrixName(String modelName, String txtName) {
        mEGLModelMartrixName = modelName;
        mEGLTextureMatrixName = txtName;
    }

    /**
     *
     */
    @Override
    public void onOpen() {
        mCameraManager.startPreview(mSurfaceTexture);
    }

    /**
     * @param error
     * @param msg
     */
    @Override
    public void onOpenError(int error, String msg) {
        Log.e(TAG, "Open Camera ErrorCode = " + error + " msg:" + msg);
    }

    /**
     * @param previewWidth
     * @param previewHeight
     */
    @Override
    public void onPreview(int previewWidth, int previewHeight) {
        if (previewWidth < 0 || previewHeight < 0) {
            throw new IllegalArgumentException("Size cannot be negative.");
        }

        mWidth = previewWidth;
        mHeight = previewHeight;

        requestLayout();
    }

    /**
     * @param mm
     */
    @Override
    public void onSetMatrix(Matrix mm) {
    }

    /**
     * @param error
     * @param msg
     */
    @Override
    public void onPreviewError(int error, String msg) {
        Log.e(TAG, "Open Camera Preview ErrorCode = " + error + " msg:" + msg);
    }

    /**
     *
     */
    @Override
    public void onClose() {
        mCameraManager.closeCamera();
    }

    private void onGLSurfaceCreated(SurfaceTexture surfaceTexture) {
        mSurfaceTexture = surfaceTexture;
        mSurfaceTexture.setOnFrameAvailableListener(new SurfaceTexture.OnFrameAvailableListener() {
            @Override
            public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                // send message to render thread to draw
                if (mRenderThread != null) {
                    RenderOESCameraHandler handler = mRenderThread.getRenderHandler();
                    if (handler != null) {
                        handler.sendFrameAvailable();
                    }
                }
            }
        });

        openCamera();
    }

    private void openCamera() {
        if (mSurfaceTexture == null) {
            Log.v(TAG, "SurfaceHolder is null.");
            return;
        }

        if (mCameraManager.isOpen()) {
            Log.v(TAG, "Camera is opened.");
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                mCameraManager.openCamera(mWidth, mHeight);
                // Now, EGL had created, we should create GLProgram and set some uniform variable
                setProgram();
            }
        }).start();
    }

    private void setProgram() {
        if (mRenderThread != null) {
            RenderOESCameraHandler handler = mRenderThread.getRenderHandler();
            if (handler != null) {
                if (!mhadSetProgram) {
                    if (mVertStr != null && mFragStr != null)
                        handler.setShaderString(mProgramName, mVertStr, mFragStr);
                    else if (mVertName != null && mFragName != null)
                        handler.sendShaderName(mProgramName, mVertName, mFragName);
                    else
                        return;
                    mhadSetProgram = true;
                }

                float[] modelMatrix = new float[16];
                setIdentityM(modelMatrix, 0);

                // Note: camera2 api did not set rotation matrix for SurfaceTexture, so need do transform in here
                if (mCameraManager instanceof Camera2Manager) {
                    int displayRotation = mCameraManager.getDisplayOrientation();
                    int degree = 0;
                    switch (displayRotation) {
                        case 0:
                            degree = 90;
                            break;
                        case 90:
                            degree = 0;
                            break;
                        case 180:
                            degree = 270;
                            break;
                        case 270:
                            degree = 180;
                            break;
                    }
                    setRotateM(modelMatrix, 0, degree, 0, 0, 1);
                }

                handler.sendUniform(mEGLModelMartrixName, modelMatrix);
                // texture matrix should been get in render thread, so only set it's name
                handler.setTextureMatrixName(mEGLTextureMatrixName);
            }
        }
    }

}
