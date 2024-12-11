package com.bisw.camera.base;

import android.content.Context;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.util.Log;
import android.view.TextureView;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public abstract class BaseCameraTextureView extends TextureView implements
        TextureView.SurfaceTextureListener, BaseView, CameraCallback{
    private static final String TAG = BaseCameraTextureView.class.getSimpleName();
    private SurfaceTexture mSurface;
    private int mWidth;
    private int mHeight;
    private ICameraManager mCameraManager;

    public BaseCameraTextureView(@NonNull Context context) {
        this(context, null);
    }

    public BaseCameraTextureView(@NonNull Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public BaseCameraTextureView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        this(context, attrs, defStyleAttr, 0);
    }

    public BaseCameraTextureView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(context);
    }

    public abstract ICameraManager createCameraManager(Context context);

    private void init(Context context) {
        mCameraManager = createCameraManager(context);
        mCameraManager.setCameraCallback(this);
        this.setSurfaceTextureListener(this);
    }

    /**
     * @param surfaceTexture
     * @param i
     * @param i1
     */
    @Override
    public void onSurfaceTextureAvailable(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {
        mSurface = surfaceTexture;
        mWidth = i;
        mHeight = i1;
        openCamera();
    }

    /**
     * @param surfaceTexture
     * @param i
     * @param i1
     */
    @Override
    public void onSurfaceTextureSizeChanged(@NonNull SurfaceTexture surfaceTexture, int i, int i1) {

    }

    /**
     * @param surfaceTexture
     * @return
     */
    @Override
    public boolean onSurfaceTextureDestroyed(@NonNull SurfaceTexture surfaceTexture) {
        mCameraManager.releaseCamera();
        return true;
    }

    /**
     * @param surfaceTexture
     */
    @Override
    public void onSurfaceTextureUpdated(@NonNull SurfaceTexture surfaceTexture) {

    }

    // Interface for outside called
    public void Pause() {
        mCameraManager.releaseCamera();
    }

    public void Resume() {
        openCamera();
    }

    public void Close() {
        mCameraManager.releaseCamera();
    }

    public ICameraManager getCameraManager() {
        return mCameraManager;
    }

    public SurfaceTexture getSurfaceTexture() {
        return mSurface;
    }

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

    private void openCamera() {
        if (mSurface == null) {
            Log.v(TAG, "Surface is null.");
            return;
        }

        if (mCameraManager.isOpen()) {
            Log.v(TAG, "Camera is opened.");
            return;
        }

        new Thread(new OpenCameraRunnable()).start();
    }

    /**
     *
     */
    @Override
    public void onOpen() {
        mCameraManager.startPreview(mSurface);
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
        setTransform(mm);
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

    private class OpenCameraRunnable implements Runnable {
        @Override
        public void run() {
            mCameraManager.openCamera(mWidth, mHeight);
        }
    }

}
