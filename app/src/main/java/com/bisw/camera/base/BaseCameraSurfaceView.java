package com.bisw.camera.base;

import android.content.Context;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import androidx.annotation.NonNull;

public abstract class BaseCameraSurfaceView extends SurfaceView implements SurfaceHolder.Callback, BaseView, CameraCallback {
    private static final String TAG = BaseCameraSurfaceView.class.getSimpleName();
    private Context mContext;
    private SurfaceHolder mSurfaceHolder;
    private int mWidth;
    private int mHeight;
    private ICameraManager mCameraManager;

    public BaseCameraSurfaceView(Context context) {
        this(context, null);
    }

    public BaseCameraSurfaceView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public BaseCameraSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public BaseCameraSurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        mContext = context;
        mSurfaceHolder = getHolder();
        mSurfaceHolder.setFormat(PixelFormat.TRANSPARENT);
        mSurfaceHolder.addCallback(this);
        mCameraManager = createCameraManager(context);
        mCameraManager.setCameraCallback(this);
        mWidth = mHeight = 0;
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
            openCamera();
        }
    }

    /**
     * @param surfaceHolder
     * @param i
     * @param i1
     * @param i2
     */
    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    /**
     * @param surfaceHolder
     */
    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        Log.v(TAG, "surfaceDestroyed.");
        mCameraManager.releaseCamera();
    }

    /**
     *
     */
    @Override
    public void onOpen() {
        mCameraManager.startPreview(mSurfaceHolder);
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
        };
    }

    // Interface for outside called
    public void Pause() {
        mCameraManager.releaseCamera();
    }

    public void Resume() {
        if(mWidth > 0 && mHeight > 0)
            openCamera();
    }

    public void Close() {
        mCameraManager.releaseCamera();
    }

    public ICameraManager getCameraManager() {
        return mCameraManager;
    }

    public SurfaceHolder getSurfaceHolder() {
        return mSurfaceHolder;
    }

    public abstract ICameraManager createCameraManager(Context context);

    private void openCamera() {
        if (mSurfaceHolder == null) {
            Log.v(TAG, "SurfaceHolder is null.");
            return;
        }

        if (mCameraManager.isOpen()) {
            Log.v(TAG, "Camera is opened.");
            return;
        }

        new Thread(new OpenCameraRunnable()).start();
    }

    private class OpenCameraRunnable implements Runnable {
        @Override
        public void run() {
            mCameraManager.openCamera(mWidth, mHeight);
        }
    }
}
