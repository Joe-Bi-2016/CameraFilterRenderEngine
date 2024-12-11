package com.bisw.camera.view;

import android.content.Context;
import android.util.AttributeSet;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import com.bisw.camera.base.BaseCameraTextureView;
import com.bisw.camera.base.CameraManager;
import com.bisw.camera.base.ICameraManager;

public class CameraTextureView extends BaseCameraTextureView {
    public CameraTextureView(@NonNull Context context) {
        super(context);
    }

    public CameraTextureView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    public CameraTextureView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public CameraTextureView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    /**
     * @param context
     * @return
     */
    @Override
    public ICameraManager createCameraManager(Context context) {
        return new CameraManager(context);
    }
}
