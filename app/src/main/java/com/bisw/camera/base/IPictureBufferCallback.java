package com.bisw.camera.base;

import android.content.Context;
import android.graphics.Bitmap;

public interface IPictureBufferCallback {
    void onPictureToken(Bitmap bitmap, String savedPath, Context context);
}
