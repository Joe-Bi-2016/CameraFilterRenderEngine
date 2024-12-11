package com.bisw.camera.view;

import android.content.Context;
import android.util.AttributeSet;
import com.bisw.camera.base.BaseCameraSurfaceView;
import com.bisw.camera.base.Camera2Manager;
import com.bisw.camera.base.ICameraManager;

public class Camera2SurfaceView extends BaseCameraSurfaceView {
    public Camera2SurfaceView(Context context) {
        super(context);
    }

    public Camera2SurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public Camera2SurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public Camera2SurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
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
