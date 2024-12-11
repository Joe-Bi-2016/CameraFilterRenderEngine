package com.bisw.camera.view;

import android.content.Context;
import android.util.AttributeSet;
import com.bisw.camera.base.BaseCameraSurfaceView;
import com.bisw.camera.base.CameraManager;
import com.bisw.camera.base.ICameraManager;

public class CameraSurfaceView extends BaseCameraSurfaceView {
    public CameraSurfaceView(Context context) {
        super(context);
    }

    public CameraSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public CameraSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public CameraSurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
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
