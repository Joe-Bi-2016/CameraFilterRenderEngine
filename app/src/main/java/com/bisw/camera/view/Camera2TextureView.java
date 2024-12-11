package com.bisw.camera.view;

import android.content.Context;
import android.util.AttributeSet;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import com.bisw.camera.base.BaseCameraTextureView;
import com.bisw.camera.base.Camera2Manager;
import com.bisw.camera.base.ICameraManager;

public class Camera2TextureView extends BaseCameraTextureView {
    public Camera2TextureView(@NonNull Context context) {
        super(context);
    }

    public Camera2TextureView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    public Camera2TextureView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public Camera2TextureView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    /**
     * @param context
     * @return
     */
    @Override
    public ICameraManager createCameraManager(Context context) {
        return new Camera2Manager(context);
    }
}
