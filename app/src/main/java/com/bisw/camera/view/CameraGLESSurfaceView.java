package com.bisw.camera.view;

import android.content.Context;
import android.util.AttributeSet;
import com.bisw.camera.base.BaseGLESCameraSurfaceView;
import com.bisw.camera.base.CameraManager;
import com.bisw.camera.base.ICameraManager;

public class CameraGLESSurfaceView extends BaseGLESCameraSurfaceView {
    public CameraGLESSurfaceView(Context context) {
        super(context);
    }

    public CameraGLESSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public CameraGLESSurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public CameraGLESSurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
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

    /**
     * @param vert
     * @param frag
     */
    @Override
    public void setShaderName(String programName, String vert, String frag) {
        mProgramName = programName;
        mVertName = vert;
        mFragName = frag;
        mhadSetProgram = false;
    }

    /**
     * @param vert
     * @param frag
     */
    @Override
    public void setShaderString(String programName, String vert, String frag) {
        mProgramName = programName;
        mVertStr = vert;
        mFragStr = frag;
        mhadSetProgram = false;
    }

}
