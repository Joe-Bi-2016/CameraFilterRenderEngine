package com.bisw.camera.base;

import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.os.Handler;
import android.view.SurfaceHolder;
import android.util.Size;

public interface ICameraManager {
    void setCameraId(int cameraId);

    int getCameraId();

    void openCamera(int viewWidth, int viewHeight);

    boolean isOpen();

    void closeCamera();

    void startPreview(SurfaceHolder surfaceHolder);

    void startPreview(SurfaceTexture surfaceTexture);

    void stopPreview();

    void releaseCamera();

    Size getPreviewSize();

    int getOrientation();

    int getDisplayOrientation();

    void setCameraCallback(CameraCallback cameraCallback);

    void addPreviewBufferCallback(IPreviewBufferCallback IPreviewBufferCallback);

    void takePicture(IPictureBufferCallback IPictureBufferCallback);

    void switchCamera();

    void rotateDevice();

    boolean isSupportedZoom();

    void setZoom(int zoomValue);

    int getZoom();

    void handleZoom(boolean isZoomIn);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // default method
    default void onOpen(CameraCallback cameraCallback, Handler handler) {
        if (handler != null) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    if (cameraCallback != null) {
                        cameraCallback.onOpen();
                    }
                }
            });
        } else{
            if (cameraCallback != null) {
                cameraCallback.onOpen();
            }
        }
    }

    default void onOpenError(CameraCallback cameraCallback, Handler handler, int error, String msg) {
        if (handler != null) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    if (cameraCallback != null) {
                        cameraCallback.onOpenError(error, msg);
                    }
                }
            });
        } else {
            if (cameraCallback != null) {
                cameraCallback.onOpenError(error, msg);
            }
        }
    }

    default void onPreview(CameraCallback cameraCallback, Handler handler, int width, int height) {
        if (handler != null) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    if (cameraCallback != null) {
                        cameraCallback.onPreview(width, height);
                    }
                }
            });
        } else {
            if (cameraCallback != null) {
                cameraCallback.onPreview(width, height);
            }
        }
    }

    default void onPreviewError(CameraCallback cameraCallback, Handler handler, int err, String msg) {
        if (handler != null) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    if (cameraCallback != null) {
                        cameraCallback.onPreviewError(err, msg);
                    }
                }
            });
        } else {
            if (cameraCallback != null) {
                cameraCallback.onPreviewError(err, msg);
            }
        }
    }

    default void onSetMatrix(CameraCallback cameraCallback, Handler handler, Matrix mm) {
        if (handler != null) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    if (cameraCallback != null) {
                        cameraCallback.onSetMatrix(mm);
                    }
                }
            });
        } else {
            if (cameraCallback != null) {
                cameraCallback.onSetMatrix(mm);
            }
        }
    }

    default void onClose(CameraCallback cameraCallback, Handler handler) {
        if (handler != null) {
            handler.post(new Runnable() {
                @Override
                public void run() {
                    if (cameraCallback != null) {
                        cameraCallback.onClose();
                    }
                }
            });
        } else {
            if (cameraCallback != null) {
                cameraCallback.onClose();
            }
        }
    }


}
