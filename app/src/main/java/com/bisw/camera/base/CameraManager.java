package com.bisw.camera.base;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Handler;
import android.util.Log;
import android.util.Size;
import android.view.OrientationEventListener;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import android.graphics.Matrix;
import java.util.ArrayList;
import java.util.List;

public class CameraManager implements Camera.AutoFocusCallback, ICameraManager{
    private static final String TAG = CameraManager.class.getSimpleName();
    public static final int CAMERA_ERROR_NO_ID = -1001;
    public static final int CAMERA_ERROR_OPEN = -1002;
    public static final int CAMERA_ERROR_PREVIEW = -2001;
    private Camera mCamera;
    private int mViewWidth;
    private int mViewHeight;
    private Camera.Parameters mParameters;
    private Camera.CameraInfo mCameraInfo = new Camera.CameraInfo();
    private boolean mIsPreviewing = false;
    private int mDisplayOrientation = -1;
    private int mOrientation = -1;
    private int mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
    private int mPreviewWidth;
    private int mPreviewHeight;
    private Context mContext;
    private byte[] mCameraBytes = null;
    private boolean isSupportZoom;
    private CameraCallback mCameraCallback;
    private List<IPreviewBufferCallback> mPreviewBufferCallbacks = new ArrayList<>();
    private IPictureBufferCallback mPictureBufferCallback;
    private OrientationEventListener mOrientationEventListener;
    private int mLastRotation = 0;
    private Handler mUIHandler;
    private Matrix mMatrix;

    // preview callback
    private Camera.PreviewCallback mPreviewCallback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] bytes, Camera camera) {
            if (!mPreviewBufferCallbacks.isEmpty()) {
                for (IPreviewBufferCallback previewBufferCallback : mPreviewBufferCallbacks) {
                    previewBufferCallback.onPreviewBufferFrame(bytes, mPreviewWidth, mPreviewHeight, IPreviewBufferCallback.YUVFormat.NV21);
                }
            }
            mCameraBytes = bytes;
            camera.addCallbackBuffer(bytes);
        }
    };

    // tone of taking picture
    private Camera.ShutterCallback mShutterCallback = new Camera.ShutterCallback() {
        @Override
        public void onShutter() {
            Log.v(TAG, "myShutterCallback: onShutter...");
        }
    };

    // compress callback of taking picture
    private Camera.PictureCallback mRawCallback = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] bytes, Camera camera) {
            Log.v(TAG, "myRawCallback: onPictureTaken...");
        }
    };

    // take picture callback
    private Camera.PictureCallback mPictureCallback = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] bytes, Camera camera) {
            camera.startPreview();
            Bitmap bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
            bitmap = Utils.rotateBitmap(mCameraId, mDisplayOrientation, bitmap);
            String path = String.valueOf(mContext.getExternalFilesDir(null));
            mPictureBufferCallback.onPictureToken(bitmap, path, mContext);
        }
    };

    private Camera.ErrorCallback errorCallback = new Camera.ErrorCallback() {
        @Override
        public void onError(int i, Camera camera) {
            String msg = "";
            switch(i) {
                case Camera.CAMERA_ERROR_SERVER_DIED:
                    Log.e(TAG, "Camera.CAMERA_ERROR_SERVER_DIED camera id: " + mCameraId);
                    msg = "Camera Media server died.";
                    break;
                case Camera.CAMERA_ERROR_UNKNOWN:
                    Log.e(TAG, "Camera.CAMERA_ERROR_UNKNOWN");
                    msg = "Camera unkown error.";
                    break;
            }
            releaseCamera();
        }
    };

    public CameraManager(Context context) {
        mContext = context;
        mUIHandler = new Handler(mContext.getMainLooper());
        mOrientationEventListener = new OrientationEventListener(context) {
            @Override
            public void onOrientationChanged(int i) {
                setPictureRotate(i);
            }
        };
    }

    public Camera getCamera() {
        return mCamera;
    }

    /**
     * @param cameraId
     */
    @Override
    public void setCameraId(int cameraId) {
        mCameraId = cameraId;
    }

    /**
     * @return
     */
    @Override
    public int getCameraId() {
        return mCameraId;
    }

    /**
     *
     */
    @Override
    public synchronized void openCamera(int viewWidth, int viewHeight) {
        if (isPreviewing())
            return;
        Log.v(TAG, "Camera open #" + mCameraId);
        if (mCamera == null) {
            if (mCameraId >= Camera.getNumberOfCameras()) {
                onOpenError(mCameraCallback, mUIHandler, CAMERA_ERROR_NO_ID, "No Camera.");
                return;
            }
        }

        mPreviewWidth = 1920;
        mPreviewHeight = 1080;

        try {
            mCamera = Camera.open(mCameraId);
            if (mCamera != null) {
                Camera.getCameraInfo(mCameraId, mCameraInfo);
                mCamera.setErrorCallback(errorCallback);
                mViewWidth = viewWidth;
                mViewHeight = viewHeight;
                initCamera(viewWidth, viewHeight);
                mOrientationEventListener.enable();
                onOpen(mCameraCallback, mUIHandler);
            }
        }catch (Exception e) {
            onOpenError(mCameraCallback, mUIHandler, CAMERA_ERROR_OPEN, e.getMessage());
        }
    }

    /**
     * @return
     */
    @Override
    public synchronized boolean isOpen() {
        return mCamera != null;
    }

    /**
     *
     */
    @Override
    public synchronized void closeCamera() {

    }

    /**
     * @param surfaceHolder
     */
    @Override
    public synchronized void startPreview(SurfaceHolder surfaceHolder) {
        Log.v(TAG, "startview...");
        if (mIsPreviewing)
            return;
        if (mCamera != null) {
            try {
                mCamera.setPreviewDisplay(surfaceHolder);
                if (!mPreviewBufferCallbacks.isEmpty()) {
                    mCamera.addCallbackBuffer(new byte[mPreviewWidth * mPreviewHeight * 3 / 2]);
                    mCamera.setPreviewCallbackWithBuffer(mPreviewCallback);
                }
                mCamera.startPreview();
                mIsPreviewing = true;
                onPreview(mCameraCallback, mUIHandler, mPreviewWidth, mPreviewHeight);
                // camera1 api had do ratate, so does not set matrix of rotation
//                onSetMatrix(mCameraCallback, mUIHandler, mMatrix);
            } catch (Exception e) {
                onPreviewError(mCameraCallback, mUIHandler, CAMERA_ERROR_PREVIEW, e.getMessage());
            }
        }
    }

    /**
     * @param surfaceTexture
     */
    @Override
    public synchronized void startPreview(SurfaceTexture surfaceTexture) {
        Log.v(TAG, "startview...");
        if (mIsPreviewing)
            return;
        if (mCamera != null) {
            try {
                surfaceTexture.setDefaultBufferSize(mPreviewWidth, mPreviewHeight);
                mCamera.setPreviewTexture(surfaceTexture);
                if (!mPreviewBufferCallbacks.isEmpty()) {
                    mCamera.addCallbackBuffer(new byte[mPreviewWidth * mPreviewHeight * 3 / 2]);
                    mCamera.setPreviewCallbackWithBuffer(mPreviewCallback);
                }
                mCamera.startPreview();
                mIsPreviewing = true;
                onPreview(mCameraCallback, mUIHandler, mPreviewWidth, mPreviewHeight);
                // camera1 api had do ratate, so does not set matrix of rotation
//                onSetMatrix(mCameraCallback, mUIHandler, mMatrix);
            } catch (Exception e) {
                onPreviewError(mCameraCallback, mUIHandler, CAMERA_ERROR_PREVIEW, e.getMessage());
            }
        }
    }

    /**
     *
     */
    @Override
    public synchronized void stopPreview() {
        Log.v(TAG, "stopPreivew.");
        if (mIsPreviewing && mCamera != null) {
            try {
                mCamera.setPreviewCallback(null);
                mCamera.stopPreview();
                mPreviewBufferCallbacks.clear();
            } catch (Exception e) {
                e.printStackTrace();
            }
            mIsPreviewing = false;
        }
    }

    /**
     *
     */
    @Override
    public synchronized void releaseCamera() {
        Log.v(TAG, "releaseCamera.");
        if (mCamera != null) {
            stopPreview();
            mOrientationEventListener.disable();
            try {
                mCamera.release();
                mCamera = null;
                mCameraBytes = null;
                mDisplayOrientation = -1;
            } catch (Exception e) {
            }
            onClose(mCameraCallback, mUIHandler);
        }
    }

    /**
     * @return
     */
    @Override
    public Size getPreviewSize() {
        return new Size(mPreviewWidth, mPreviewHeight);
    }

    /**
     * @return
     */
    @Override
    public int getOrientation() {
        return mOrientation;
    }

    /**
     * @return
     */
    @Override
    public int getDisplayOrientation() {
        return mDisplayOrientation;
    }

    /**
     * @param cameraCallback
     */
    @Override
    public void setCameraCallback(CameraCallback cameraCallback) {
        mCameraCallback = cameraCallback;
    }

    /**
     * @param IPreviewBufferCallback
     */
    @Override
    public void addPreviewBufferCallback(IPreviewBufferCallback IPreviewBufferCallback) {
        if (IPreviewBufferCallback != null && !mPreviewBufferCallbacks.contains(IPreviewBufferCallback))
            mPreviewBufferCallbacks.add(IPreviewBufferCallback);
    }

    /**
     * @param IPictureBufferCallback
     */
    @Override
    public void takePicture(IPictureBufferCallback IPictureBufferCallback) {
        mPictureBufferCallback = IPictureBufferCallback;
        takePicture(mShutterCallback, mRawCallback, mPictureCallback);
    }

    /**
     *
     */
    @Override
    public void switchCamera() {
        if (mCameraId == Camera.CameraInfo.CAMERA_FACING_BACK) {
            mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        } else {
            mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        }

        List<IPreviewBufferCallback> previewbuffercallback = new ArrayList<>();
        if (!mPreviewBufferCallbacks.isEmpty())
            previewbuffercallback.addAll(mPreviewBufferCallbacks);
        releaseCamera();
        if (!previewbuffercallback.isEmpty())
            mPreviewBufferCallbacks.addAll(previewbuffercallback);
        openCamera(mViewWidth, mViewHeight);
    }

    /**
     *
     */
    @Override
    public void rotateDevice() {
        getCameraDisplayAndOrientation(mContext, mCameraId, mCamera);
        mCamera.setDisplayOrientation(mDisplayOrientation);
    }

    /**
     * @param b
     * @param camera
     * @deprecated
     */
    @Override
    public void onAutoFocus(boolean b, Camera camera) {

    }

    public int getLastRotation() {
        return mLastRotation;
    }

    public boolean isPreviewing() {
        return mIsPreviewing;
    }

    public boolean isSupportedZoom() {
        return isSupportZoom;
    }

    public void setZoom(int zoomValue) {
        if (isSupportZoom) {
            try {
                Camera.Parameters params = mCamera.getParameters();
                final int MAX = params.getMaxZoom();
                if (MAX == 0) return;
                params.setZoom(zoomValue);
                mCamera.setParameters(params);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public int getZoom() {
        if (isSupportZoom) {
            try {
                Camera.Parameters params = mCamera.getParameters();
                return params.getZoom();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        return 0;
    }

    public synchronized void handleZoom(boolean isZoomIn) {
        if (mCamera == null)
            return;
        Camera.Parameters params = mCamera.getParameters();
        if (params.isZoomSupported()) {
            int maxZoom = params.getMaxZoom();
            int zoom = params.getZoom();
            if (isZoomIn && zoom < maxZoom) {
                zoom++;
            } else if (zoom > 0) {
                zoom--;
            }
            Log.d(TAG, "handleZoom: zoom: " + zoom);
            params.setZoom(zoom);
            mCamera.setParameters(params);
        } else {
            Log.i(TAG, "zoom not supported");
        }
    }

    private void initCamera(int viewWidth, int viewHeight) {
        if (mCamera != null) {
            mParameters = mCamera.getParameters();

            if (mDisplayOrientation == -1)
                getCameraDisplayAndOrientation(mContext, mCameraId, mCamera);

            mParameters.setRotation(mOrientation);
            mCamera.setDisplayOrientation(mDisplayOrientation);

            List<String> supportFlashModes = mParameters.getSupportedFlashModes();
            if (supportFlashModes != null && supportFlashModes.contains(Camera.Parameters.FLASH_MODE_OFF))
                mParameters.setFlashMode(Camera.Parameters.FLASH_MODE_OFF);

            List<String> supportFocusModes = mParameters.getSupportedFocusModes();
            if (supportFocusModes != null && supportFocusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
                mParameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);

            mParameters.setPreviewFormat(ImageFormat.NV21);
            mParameters.setPictureFormat(ImageFormat.JPEG);
            mParameters.setExposureCompensation(0);

//            Camera.Size previewSize = Utils.getOptimalSize(mParameters.getSupportedPreviewSizes(), mPreviewWidth, mPreviewHeight);
            Camera.Size previewSize = Utils.getOptimalSize(mParameters.getSupportedPreviewSizes(), viewWidth, viewHeight);

            int rotation = ((Activity)(mContext)).getWindowManager().getDefaultDisplay().getRotation();
            mMatrix = Utils.calculateSurfaceTransform(rotation, mPreviewWidth, mPreviewHeight, previewSize.width, previewSize.height);

//            Camera.Size pictureSize = Utils.getOptimalSize(mParameters.getSupportedPictureSizes(), mPreviewWidth, mPreviewHeight);
            Camera.Size pictureSize = Utils.getOptimalSize(mParameters.getSupportedPictureSizes(), viewWidth, viewHeight);
            mParameters.setPictureSize(pictureSize.width, pictureSize.height);

            mPreviewWidth = previewSize.width;
            mPreviewHeight = previewSize.height;
            mParameters.setPreviewSize(mPreviewWidth, mPreviewHeight);

            isSupportZoom = mParameters.isSmoothZoomSupported();

            mCamera.setParameters(mParameters);
        }
    }

    private void getCameraDisplayAndOrientation(Context context, int cameraId, Camera camera) {
        if (context == null)
            return;
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        WindowManager wms = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        int rotate = wms.getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotate) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }

        // set camera preview rotate degree
        int ret;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            ret = (info.orientation + degrees) % 360;
            ret = (360 - ret) % 360;
        } else {
            ret = (info.orientation - degrees + 360) % 360;
        }

        mDisplayOrientation = ret;
        mOrientation = info.orientation;
    }

    private void setFlashMode(boolean on) {
        if (mCamera != null) {
            Camera.Parameters param = mCamera.getParameters();
            List<String> flashModes = param.getSupportedFlashModes();
            if (flashModes == null)
                return;

            String flashMode = param.getFlashMode();
            if (on) {
                if (!Camera.Parameters.FLASH_MODE_TORCH.equals(flashMode)) {
                    if (flashModes.contains(Camera.Parameters.FLASH_MODE_TORCH)) {
                        param.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
                        mCamera.setParameters(param);
                    }
                }
            } else {
                if (!Camera.Parameters.FLASH_MODE_OFF.equals(flashMode)) {
                    if (flashModes.contains(Camera.Parameters.FLASH_MODE_OFF)) {
                        param.setFlashMode(Camera.Parameters.FLASH_MODE_OFF);
                        mCamera.setParameters(param);
                    }
                }
            }
        }
    }

    private void setPictureRotate(int orientation) {
        if (orientation == OrientationEventListener.ORIENTATION_UNKNOWN)
            return;
        orientation = (orientation + 45) / 90 * 90;
        int rotation;
        if (mCameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT)
            rotation = (mCameraInfo.orientation - orientation + 360) % 360;
        else
            rotation = (mCameraInfo.orientation + orientation) % 360;
        mLastRotation = rotation;
    }

    private void takePicture(Camera.ShutterCallback shutterCallback, Camera.PictureCallback rawCallback, Camera.PictureCallback jpegCallback) {
        if (jpegCallback == null) {
            Log.v(TAG, "Camera.PictureCallback is null");
            return;
        }

        if (null != mCamera && mIsPreviewing) {
            mIsPreviewing = false;
            Log.v(TAG, "mLastRotation:" + mLastRotation);
            mParameters.setRotation(mLastRotation);
            mCamera.setParameters(mParameters);
            mCamera.takePicture(shutterCallback, rawCallback, jpegCallback);
        }
    }

}
