package com.bisw.camera.base;

import android.graphics.Matrix;

public interface CameraCallback {
    void onOpen();

    void onOpenError(int error, String msg);

    void onPreview(int previewWidth, int previewHeight);

    void onSetMatrix(Matrix mm);

    void onPreviewError(int error, String msg);

    void onClose();
}
