package com.bisw.camera.base;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.media.MediaActionSound;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.OrientationEventListener;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.locks.ReentrantLock;

public class Camera2Manager implements ICameraManager{
    private static final String TAG = Camera2Manager.class.getSimpleName();
    public static final int CAMERA_ERROR_NO_ID = -1001;
    public static final int CAMERA_ERROR_OPEN = -1002;
    public static final int CAMERA_ERROR_PREVIEW = -2001;
    private int mCameraId = CameraCharacteristics.LENS_FACING_BACK;
    private Context mContext;
    private int mFacing;
    private CameraCharacteristics mCameraCharacteristics;
    private CameraDevice mCameraDevice;
    private int mViewWidth;
    private int mViewHeight;
    private OrientationEventListener mOrientationEventListener;
    private Handler mBackgroundHandler;
    private HandlerThread mBackgroundThread;
    private ImageReader mPictureImageReader;
    private CameraCaptureSession mCameraSession;
    private CaptureRequest.Builder mPreviewRequestBuilder;
    private int mSensorOrientation;
    private Surface mPreviewSurface;
    private static final int MAX_PREVIEW_WIDTH = 1920;
    private static final int MAX_PREVIEW_HEIGHT = 1080;
    private int mPreviewWidth;
    private int mPreviewHeight;
    private Matrix mMatrix;
    private List<IPreviewBufferCallback> mPreviewBufferCallbacks = new ArrayList<>();
    private IPictureBufferCallback mPictureBufferCallback;
    private Size mPictureSize;
    private static final int STATE_PREVIEW = 0; // Camera state: Showing camera preview
    private static final int STATE_WAITING_LOCK = 1; // Camera state: Waiting for the focus to be locked
    private static final int STATE_WAITING_PRECAPTURE = 2; // Camera state: Waiting for the exposure to be precapture state
    private static final int STATE_WAITING_NON_PRECAPTURE = 3; // Camera state: Waiting for the exposure state to be something other than precapture
    private static final int STATE_PICTURE_TAKEN = 4; // Camera state: Picture was taken
    private static final MediaActionSound mediaActionSound = new MediaActionSound();
    private int mState = STATE_PREVIEW;
    private int mDisplayRotation = 0;
    private int mDeviceOrientation = 0;
    private int mLastRotation = 0;
    private CameraCallback mCameraCallback;
    private Handler mUIHandler;
    private boolean mIsPreviewing;
    private boolean mFlashSupported;
    private boolean isSupportZoom;
    private final int MAX_ZOOM = 200;
    private int mZoom = 0;
    private float mStepWidth;
    private float mStepHeight;

    private CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            Log.v(TAG, "onOpened");
            mCameraDevice = cameraDevice;
            onOpen(mCameraCallback, mUIHandler);
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            Log.v(TAG, "onDisconnected");
            releaseCamera();
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int i) {
            Log.e(TAG, "Camera open error, errorCode = " + i);
            releaseCamera();
        }
    };

    private ImageReader.OnImageAvailableListener mOnCaptureImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            ByteBuffer buffer = reader.acquireNextImage().getPlanes()[0].getBuffer();
            byte[] bytes = new byte[buffer.remaining()];
            buffer.get(bytes);
            Bitmap bitmap = BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
            bitmap = Utils.rotateBitmap((mCameraId ^ 1), mDisplayRotation, bitmap);
            String path = String.valueOf(mContext.getExternalFilesDir(null));
            mPictureBufferCallback.onPictureToken(bitmap, path, mContext);
        }
    };

    private CameraCaptureSession.CaptureCallback mCaptureCallback = new CameraCaptureSession.CaptureCallback() {
        private void process(CaptureResult result) {
            switch (mState) {
                case STATE_PREVIEW: {
                    // We have nothing to do when the camera preview is working normally.
                    break;
                }
                case STATE_WAITING_LOCK: {
                    Integer afState = result.get(CaptureResult.CONTROL_AF_STATE);
                    if (afState == null) {
                        captureStillPicture();
                    } else if (CaptureResult.CONTROL_AF_STATE_FOCUSED_LOCKED == afState ||
                            CaptureResult.CONTROL_AF_STATE_NOT_FOCUSED_LOCKED == afState) {
                        // CONTROL_AE_STATE can be null on some devices
                        Integer aeState = result.get(CaptureResult.CONTROL_AE_STATE);
                        if (aeState == null ||
                                aeState == CaptureResult.CONTROL_AE_STATE_CONVERGED) {
                            mState = STATE_PICTURE_TAKEN;
                            captureStillPicture();
                        } else {
                            runPrecaptureSequence();
                        }
                    }
                    break;
                }
                case STATE_WAITING_PRECAPTURE: {
                    // CONTROL_AE_STATE can be null on some devices
                    Integer aeState = result.get(CaptureResult.CONTROL_AE_STATE);
                    if (aeState == null ||
                            aeState == CaptureResult.CONTROL_AE_STATE_PRECAPTURE ||
                            aeState == CaptureRequest.CONTROL_AE_STATE_FLASH_REQUIRED) {
                        mState = STATE_WAITING_NON_PRECAPTURE;
                    }
                    break;
                }
                case STATE_WAITING_NON_PRECAPTURE: {
                    // CONTROL_AE_STATE can be null on some devices
                    Integer aeState = result.get(CaptureResult.CONTROL_AE_STATE);
                    if (aeState == null || aeState != CaptureResult.CONTROL_AE_STATE_PRECAPTURE) {
                        mState = STATE_PICTURE_TAKEN;
                        captureStillPicture();
                    }
                    break;
                }
            }
        }

        @Override
        public void onCaptureProgressed(@NonNull CameraCaptureSession session,
                                        @NonNull CaptureRequest request,
                                        @NonNull CaptureResult partialResult) {
            process(partialResult);
        }

        @Override
        public void onCaptureStarted(@NonNull CameraCaptureSession session, @NonNull CaptureRequest request, long timestamp, long frameNumber) {
            super.onCaptureStarted(session, request, timestamp, frameNumber);
        }

        @Override
        public void onCaptureCompleted(@NonNull CameraCaptureSession session,
                                       @NonNull CaptureRequest request,
                                       @NonNull TotalCaptureResult result) {
            process(result);
        }

    };

    public Camera2Manager(Context context) {
        mContext = context;
        mUIHandler = new Handler(mContext.getMainLooper());
        mOrientationEventListener = new OrientationEventListener(mContext) {
            @Override
            public void onOrientationChanged(int orientation) {
                mDeviceOrientation = orientation;
            }
        };
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
    public void openCamera(int viewWidth, int viewHeight) {
        if (mIsPreviewing)
            return;
        Log.v(TAG, "Camera open #" + mCameraId);
        if (mCameraDevice != null) {
            return;
        }

        // permission checking
        if (ActivityCompat.checkSelfPermission(mContext, Manifest.permission.CAMERA) !=
                PackageManager.PERMISSION_GRANTED)
            return;

        mPreviewWidth = 1920;
        mPreviewHeight = 1080;

        CameraManager cameraManager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        String cameraId = getSuiteCameraId(cameraManager);
        if (cameraId == null)
            return;

        try {
            mCameraCharacteristics = cameraManager.getCameraCharacteristics(cameraId);
        } catch (CameraAccessException e) {
            e.printStackTrace();
            return;
        }

        mFacing = mCameraCharacteristics.get(CameraCharacteristics.LENS_FACING);
        Log.v(TAG, "facing：" + mFacing);

        if (!initCamera(viewWidth, viewHeight)) {
            onOpenError(mCameraCallback, mUIHandler, CAMERA_ERROR_OPEN, "Config camera error.");
            return;
        }

        mOrientationEventListener.enable();

        startBackgroundThread();

        try {
            cameraManager.openCamera(cameraId, mStateCallback, mBackgroundHandler);
            mViewWidth = viewWidth;
            mViewHeight = viewHeight;
        } catch (CameraAccessException e) {
            Log.e(TAG, "Cannot access the camera." + e.toString());
        }
    }

    /**
     * @return
     */
    @Override
    public boolean isOpen() {
        return mCameraDevice != null;
    }

    /**
     *
     */
    @Override
    public void closeCamera() {

    }

    /**
     * @param surfaceHolder
     */
    @Override
    public void startPreview(SurfaceHolder surfaceHolder) {
        if (mIsPreviewing || !isOpen()) {
            return;
        }
        mIsPreviewing = true;
        mPreviewSurface = surfaceHolder.getSurface();
        // set size
        onPreview(mCameraCallback, mUIHandler, mPreviewWidth, mPreviewHeight);
        // set orientation
        onSetMatrix(mCameraCallback, mUIHandler, mMatrix);
        // start preview
        initPreviewRequest();
        createCommonSession();
    }

    /**
     * @param surfaceTexture
     */
    @Override
    public void startPreview(SurfaceTexture surfaceTexture) {
        if (mIsPreviewing || !isOpen()) {
            return;
        }
        mIsPreviewing = true;
        surfaceTexture.setDefaultBufferSize(mPreviewWidth, mPreviewHeight);
        mPreviewSurface = new Surface(surfaceTexture);
        // set size
        onPreview(mCameraCallback, mUIHandler, mPreviewWidth, mPreviewHeight);
        // set orientation
        onSetMatrix(mCameraCallback, mUIHandler, mMatrix);
        // start preview
        initPreviewRequest();
        createCommonSession();
    }

    /**
     *
     */
    @Override
    public void stopPreview() {
        if (mCameraSession == null) {
            Log.w(TAG, "stopPreview: mCameraSession is null");
            return;
        }
        try {
            mCameraSession.stopRepeating();
            mIsPreviewing = false;
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    /**
     *
     */
    @Override
    public void releaseCamera() {
        stopPreview();

        if (null != mCameraSession) {
            mCameraSession.close();
            mCameraSession = null;
        }
        if (mCameraDevice != null) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
        if (mPictureImageReader != null) {
            mPictureImageReader.close();
            mPictureImageReader = null;
        }

        mOrientationEventListener.disable();
        stopBackgroundThread();
        onClose(mCameraCallback, mUIHandler);
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
        return mSensorOrientation;
    }

    /**
     * @return
     */
    @Override
    public int getDisplayOrientation() {
        return mDisplayRotation;
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
        Log.v(TAG, "Should been called before startPreview");
        if (IPreviewBufferCallback != null && !mPreviewBufferCallbacks.contains(IPreviewBufferCallback)) {
            mPreviewBufferCallbacks.add(IPreviewBufferCallback);
        }
    }

    /**
     * @param IPictureBufferCallback
     */
    @Override
    public void takePicture(IPictureBufferCallback IPictureBufferCallback) {
        mPictureBufferCallback = IPictureBufferCallback;
        try {
            // This is how to tell the camera to lock focus.
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER,
                    CameraMetadata.CONTROL_AF_TRIGGER_START);
            // Tell #mCaptureCallback to wait for the lock.
            mState = STATE_WAITING_LOCK;
            mCameraSession.capture(mPreviewRequestBuilder.build(), mCaptureCallback,
                    mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    /**
     *
     */
    @Override
    public void switchCamera() {
        mCameraId ^= 1;
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
        int previewWidth = 0, previewHeight = 0;
        if (mLastRotation == Configuration.ORIENTATION_LANDSCAPE) {
            previewWidth = mPreviewWidth;
            previewHeight = mPreviewHeight;
        } else {
            previewWidth = mPreviewHeight;
            previewHeight = mPreviewWidth;
        }

        mPreviewWidth = 1920;
        mPreviewHeight = 1080;

        int displayRotation = ((Activity)(mContext)).getWindowManager().getDefaultDisplay().getRotation();
        mMatrix = Utils.calculateSurfaceTransform(displayRotation, mPreviewWidth, mPreviewHeight, previewWidth, previewHeight);

        int orientation = ((Activity)(mContext)).getResources().getConfiguration().orientation;
        if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
            mPreviewWidth = previewWidth;
            mPreviewHeight = previewHeight;
        } else {
            mPreviewWidth = previewHeight;
            mPreviewHeight = previewWidth;
        }

        mLastRotation = orientation;

        onPreview(mCameraCallback, mUIHandler, mPreviewWidth, mPreviewHeight);
        onSetMatrix(mCameraCallback, mUIHandler, mMatrix);

        // set size
        onPreview(mCameraCallback, mUIHandler, mPreviewWidth, mPreviewHeight);
        // set orientation
        onSetMatrix(mCameraCallback, mUIHandler, mMatrix);
    }

    /**
     * @return
     */
    @Override
    public boolean isSupportedZoom() {
        return isSupportZoom;
    }

    /**
     * @param zoomValue
     */
    @Override
    public void setZoom(int zoomValue) {
        if (isSupportZoom) {
            mZoom = zoomValue;
            handleZoom(true);
        }
    }

    /**
     * @return
     */
    @Override
    public int getZoom() {
        return mZoom;
    }

    /**
     * @param isZoom
     */
    @Override
    public void handleZoom(boolean isZoom) {
        if (mCameraDevice == null || mCameraCharacteristics == null || mPreviewRequestBuilder == null) {
            return;
        }
        if (isZoom && mZoom < MAX_ZOOM) // zoom out
            mZoom++;
        else if (mZoom > 0) // zoom in
            mZoom--;

        Log.v(TAG, "handleZoom: mZoom: " + mZoom);
        Rect rect = mCameraCharacteristics.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        int cropW = (int) (mStepWidth * mZoom);
        int cropH = (int) (mStepHeight * mZoom);
        Rect zoomRect = new Rect(rect.left + cropW, rect.top + cropH, rect.right - cropW, rect.bottom - cropH);
        Log.d(TAG, "zoomRect: " + zoomRect);
        mPreviewRequestBuilder.set(CaptureRequest.SCALER_CROP_REGION, zoomRect);
        startPreview();
    }

    public int getLastRotation() {
        return mLastRotation;
    }

    public boolean isFrontCmaera() {
        return mFacing == CameraMetadata.LENS_FACING_FRONT;
    }

    private String getSuiteCameraId(CameraManager cameraM) {
        String cameraId = null;
        try {
            String[] cameraList = cameraM.getCameraIdList();
            for (String id : cameraList) {
                CameraCharacteristics cameraCharacteristics = cameraM.getCameraCharacteristics(id);
                int facing = cameraCharacteristics.get(CameraCharacteristics.LENS_FACING);
                if (mCameraId == facing) {
                    cameraId = id;
                    break;
                }
            }

            if (cameraId == null) {
                onOpenError(mCameraCallback, mUIHandler, CAMERA_ERROR_NO_ID, "Camera id:" + mCameraId + " not found.");
                return null;
            }
        } catch (Exception e) {
            onOpenError(mCameraCallback, mUIHandler, CAMERA_ERROR_OPEN, e.getMessage());
            return null;
        }

        return cameraId;
    }

    private boolean initCamera(int viewWidth, int viewHeight) {
        initDisplayRotation();
        initZoomParameter();
        return configCameraParams(viewWidth, viewHeight);
    }
    private boolean configCameraParams(int viewWidth, int viewHeight) {
        // Check if the flash is supported.
        Boolean available = mCameraCharacteristics.get(CameraCharacteristics.FLASH_INFO_AVAILABLE);
        mFlashSupported = available == null ? false : available;
        StreamConfigurationMap streamConfigs = mCameraCharacteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        if (streamConfigs != null) {
            // Find out if we need to swap dimension to get the preview size relative to sensor
            // coordinate.
            int displayRotation = ((Activity)(mContext)).getWindowManager().getDefaultDisplay().getRotation();
            //noinspection ConstantConditions
            boolean swappedDimensions = false;
            switch (displayRotation) {
                case Surface.ROTATION_0:
                case Surface.ROTATION_180:
                    if (mSensorOrientation == 90 || mSensorOrientation == 270) {
                        swappedDimensions = true;
                    }
                    break;
                case Surface.ROTATION_90:
                case Surface.ROTATION_270:
                    if (mSensorOrientation == 0 || mSensorOrientation == 180) {
                        swappedDimensions = true;
                    }
                    break;
                default:
                    Log.e(TAG, "Display rotation is invalid: " + displayRotation);
            }

            Point displaySize = new Point();
            ((Activity)(mContext)).getWindowManager().getDefaultDisplay().getSize(displaySize);
            int rotatedPreviewWidth = viewWidth;
            int rotatedPreviewHeight = viewHeight;
            int maxPreviewWidth = displaySize.x;
            int maxPreviewHeight = displaySize.y;

            if (swappedDimensions) {
                rotatedPreviewWidth = viewHeight;
                rotatedPreviewHeight = viewWidth;
                maxPreviewWidth = displaySize.y;
                maxPreviewHeight = displaySize.x;
            }

            if (maxPreviewWidth > MAX_PREVIEW_WIDTH) {
                maxPreviewWidth = MAX_PREVIEW_WIDTH;
            }

            if (maxPreviewHeight > MAX_PREVIEW_HEIGHT) {
                maxPreviewHeight = MAX_PREVIEW_HEIGHT;
            }

            // Danger, W.R.! Attempting to use too large a preview size could  exceed the camera
            // bus' bandwidth limitation, resulting in gorgeous previews but the storage of
            // garbage capture data.
            Size largest = Collections.max(Arrays.asList(streamConfigs.getOutputSizes(ImageFormat.JPEG)),
                    new Utils.CompareSizesByArea());
            Size mPreviewSize = Utils.chooseOptimalSize(streamConfigs.getOutputSizes(SurfaceTexture.class),
                    rotatedPreviewWidth, rotatedPreviewHeight, maxPreviewWidth,
                    maxPreviewHeight, largest);

            mMatrix = Utils.calculateSurfaceTransform(displayRotation, mPreviewWidth, mPreviewHeight, mPreviewSize.getWidth(), mPreviewSize.getHeight());

            // We fit the aspect ratio of TextureView to the size of preview we picked.
            mLastRotation = ((Activity)(mContext)).getResources().getConfiguration().orientation;
            if (mLastRotation == Configuration.ORIENTATION_LANDSCAPE) {
                mPreviewWidth = mPreviewSize.getWidth();
                mPreviewHeight = mPreviewSize.getHeight();
            } else {
                mPreviewWidth = mPreviewSize.getHeight();
                mPreviewHeight = mPreviewSize.getWidth();
            }

            Size[] supportPictureSizes = streamConfigs.getOutputSizes(ImageFormat.JPEG);
            Size pictureSize = Collections.max(Arrays.asList(supportPictureSizes), new Utils.CompareSizesByArea());
            mPictureSize = pictureSize;
            Log.v(TAG, "pictureSize: " + pictureSize);

            return true;
        }

        return false;
    }

    private void initDisplayRotation() {
        WindowManager windowManager = (WindowManager) mContext
                .getSystemService(Context.WINDOW_SERVICE);
        int rotate = windowManager.getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotate) {
            case Surface.ROTATION_0:
                degrees = 90;
                break;
            case Surface.ROTATION_90:
                degrees = 0;
                break;
            case Surface.ROTATION_180:
                degrees = 270;
                break;
            case Surface.ROTATION_270:
                degrees = 180;
                break;
        }
        mSensorOrientation = mCameraCharacteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
        mDisplayRotation = (degrees + mSensorOrientation + 270) % 360;
        Log.d(TAG, "mDisplayRotation: " + mDisplayRotation);
    }

    private void initZoomParameter() {
        Rect rect = mCameraCharacteristics.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        Log.d(TAG, "sensor_info_active_array_size: " + rect);
        float max_digital_zoom = mCameraCharacteristics.get(CameraCharacteristics.SCALER_AVAILABLE_MAX_DIGITAL_ZOOM);
        Log.d(TAG, "max_digital_zoom: " + max_digital_zoom);
        float minWidth = rect.width() / max_digital_zoom;
        float minHeight = rect.height() / max_digital_zoom;
        mStepWidth = (rect.width() - minWidth) / MAX_ZOOM / 2;
        mStepHeight = (rect.height() - minHeight) / MAX_ZOOM / 2;
        isSupportZoom = max_digital_zoom > 0.0f;
    }

    private void startBackgroundThread() {
        mBackgroundThread = new HandlerThread("Camera2Background");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }

    private void stopBackgroundThread() {
        if (mBackgroundThread != null) {
            mBackgroundThread.quitSafely();
            try {
                mBackgroundThread.join();
                mBackgroundThread = null;
                mBackgroundHandler = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void initPreviewRequest() {
        if (mPreviewSurface == null) {
            Log.e(TAG, "initPreviewRequest failed, preview surface is null");
            return;
        }

        if (!isOpen())
            return;

        try {
            mPreviewRequestBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            // set Surface
            mPreviewRequestBuilder.addTarget(mPreviewSurface);
            // set focusMode
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE, CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
            // set explore
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE, CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);
            // set awb
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AWB_MODE, CaptureRequest.CONTROL_AWB_MODE_AUTO);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void createCommonSession() {
        List<Surface> outputs = new ArrayList<>();
        // preview output
        if (mPreviewSurface != null) {
            Log.d(TAG, "createCommonSession add target mPreviewSurface");
            outputs.add(mPreviewSurface);
        }

        // picture output
        if (mPictureSize != null && mPictureImageReader == null) {
            mPictureImageReader = ImageReader.newInstance(mPictureSize.getWidth(), mPictureSize.getHeight(), ImageFormat.JPEG, 2);
            mPictureImageReader.setOnImageAvailableListener(mOnCaptureImageAvailableListener, mBackgroundHandler);
            outputs.add(mPictureImageReader.getSurface());
        }

        try {
            mCameraDevice.createCaptureSession(outputs, new CameraCaptureSession.StateCallback() {
                @Override
                public void onConfigured(@NonNull CameraCaptureSession session) {
                    mCameraSession = session;
                    startPreview();
                }

                @Override
                public void onConfigureFailed(@NonNull CameraCaptureSession session) {
                    Log.e(TAG, "ConfigureFailed. session: " + session);
                    mIsPreviewing = false;
                }
            }, mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void startPreview() {
        if (mCameraSession == null || mPreviewRequestBuilder == null) {
            Log.w(TAG, "startPreview: mCameraSession or mPreviewRequestBuilder is null");
            return;
        }

        try {
            setAutoFlash(mPreviewRequestBuilder);
            CaptureRequest captureRequest = mPreviewRequestBuilder.build();
            mCameraSession.setRepeatingRequest(captureRequest, mCaptureCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void setAutoFlash(CaptureRequest.Builder requestBuilder) {
        if (mFlashSupported) {
            requestBuilder.set(CaptureRequest.CONTROL_AE_MODE,
                    CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);
        }
    }

    private void captureStillPicture() {
        try {
            if (null == mCameraDevice) {
                return;
            }

            mediaActionSound.play(MediaActionSound.SHUTTER_CLICK);

            // This is the CaptureRequest.Builder that we use to take a picture.
            final CaptureRequest.Builder captureBuilder =
                    mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
            captureBuilder.addTarget(mPictureImageReader.getSurface());

            // Use the same AE and AF modes as the preview.
            captureBuilder.set(CaptureRequest.CONTROL_AF_MODE,
                    CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
            setAutoFlash(captureBuilder);

            // Orientation
            int rotation = ((Activity)mContext).getWindowManager().getDefaultDisplay().getRotation();
            captureBuilder.set(CaptureRequest.JPEG_ORIENTATION, mDisplayRotation);

            // Zoom
            Rect zoomRect = mPreviewRequestBuilder.get(CaptureRequest.SCALER_CROP_REGION);
            if (zoomRect != null)
                captureBuilder.set(CaptureRequest.SCALER_CROP_REGION, zoomRect);

            CameraCaptureSession.CaptureCallback CaptureCallback
                    = new CameraCaptureSession.CaptureCallback() {
                @Override
                public void onCaptureCompleted(@NonNull CameraCaptureSession session,
                                               @NonNull CaptureRequest request,
                                               @NonNull TotalCaptureResult result) {
                    try {
                        // Reset the auto-focus trigger
                        mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_TRIGGER,
                                CameraMetadata.CONTROL_AF_TRIGGER_CANCEL);
                        setAutoFlash(mPreviewRequestBuilder);
                        mCameraSession.capture(mPreviewRequestBuilder.build(), mCaptureCallback,
                                mBackgroundHandler);
                        // After this, the camera will go back to the normal state of preview.
                        mState = STATE_PREVIEW;
                        mCameraSession.setRepeatingRequest(mPreviewRequestBuilder.build(), mCaptureCallback,
                                mBackgroundHandler);
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                }
            };

            stopPreview();
            mCameraSession.abortCaptures();
            mCameraSession.capture(captureBuilder.build(), CaptureCallback, null);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void runPrecaptureSequence() {
        try {
            // This is how to tell the camera to trigger.
            mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_PRECAPTURE_TRIGGER,
                    CaptureRequest.CONTROL_AE_PRECAPTURE_TRIGGER_START);
            // Tell #mCaptureCallback to wait for the precapture sequence to be set.
            mState = STATE_WAITING_PRECAPTURE;
            mCameraSession.capture(mPreviewRequestBuilder.build(), mCaptureCallback,
                    mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }


}
