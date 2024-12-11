package com.bisw.camera.view;

import android.content.Context;
import android.util.AttributeSet;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import com.bisw.camera.base.BaseGLESCameraTextureView;
import com.bisw.camera.base.Camera2Manager;
import com.bisw.camera.base.ICameraManager;

public class Camera2GLESTextureView extends BaseGLESCameraTextureView {
    public Camera2GLESTextureView(@NonNull Context context) {
        super(context);
    }

    public Camera2GLESTextureView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    public Camera2GLESTextureView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public Camera2GLESTextureView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes) {
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
