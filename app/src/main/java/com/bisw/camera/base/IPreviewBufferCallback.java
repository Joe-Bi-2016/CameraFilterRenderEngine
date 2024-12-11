package com.bisw.camera.base;

public interface IPreviewBufferCallback {
    public enum YUVFormat{
        I420,
        NV21
    };

    void onPreviewBufferFrame(byte[] data, int width, int height, YUVFormat format);
}
