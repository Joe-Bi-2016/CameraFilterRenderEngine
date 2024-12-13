package com.bisw.camera;

import android.Manifest;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.FrameLayout;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.bisw.camera.base.BaseGLESCameraSurfaceView;
import com.bisw.camera.base.BaseGLESCameraTextureView;
import com.bisw.camera.base.BaseView;
import com.bisw.camera.base.ICameraManager;
import com.bisw.camera.base.SavePictureCallback;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class ActualCameraView extends AppCompatActivity {
    private final String TAG = ActualCameraView.class.getSimpleName();
    private static final int ANDROID_M = 23;// Android 6.0
    private static final int ANDROID_R = 30;// Android 11.0
    private final int mRequestCode = 119;
    private View mBaseView;
    private ICameraManager mCameraManager;
    private HashMap<Integer, String[]> mShaderList;
    private int mShaderIndex = 0;

    @Override public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // request permissions
        if (Build.VERSION.SDK_INT > ANDROID_M) {
            String[] permissions = new String[]{
                    android.Manifest.permission.CAMERA,
                    android.Manifest.permission.RECORD_AUDIO,
                    android.Manifest.permission.WRITE_EXTERNAL_STORAGE,
            };

            List<String> permissionList = new ArrayList<>();
            for (int i = 0; i < permissions.length; i++) {
                if (this.checkSelfPermission(permissions[i]) != PackageManager.PERMISSION_GRANTED) {
                    if (permissions[i] == Manifest.permission.WRITE_EXTERNAL_STORAGE) {
                        if (Build.VERSION.SDK_INT < ANDROID_R) {
                            permissionList.add(permissions[i]);
                        }
                    } else {
                        permissionList.add(permissions[i]);// add
                    }
                }
            }

            String[] realNeedRequest = permissionList.toArray(new String[permissionList.size()]);
            if (realNeedRequest.length > 0) {
                this.requestPermissions(realNeedRequest, mRequestCode);
            }
        }

        mShaderList = new HashMap<Integer, String[]>();
        mShaderList.put(0, new String[]{"cameraGray", "cameraOESVertex.glsl", "cameraOESGray.glsl"});
        mShaderList.put(1, new String[]{"cameraBlur", "cameraOESVertex.glsl", "cameraOESBlue.glsl"});
        mShaderList.put(2, new String[]{"cameraAperture", "cameraOESVertex.glsl", "cameraOESAperture.glsl"});

        int layoutId = getIntent().getIntExtra("xmlName", R.layout.activity_texture_camera);

        setContentView(layoutId);
        mBaseView = (View)findViewById(R.id.cameraView);
        mCameraManager = ((BaseView)mBaseView).getCameraManager();

        String[] curShader = mShaderList.get(mShaderIndex);
        if(mBaseView instanceof BaseGLESCameraTextureView) {
            ((BaseGLESCameraTextureView) mBaseView).setShaderName(curShader[0], curShader[1], curShader[2]);
            ((BaseGLESCameraTextureView) mBaseView).setEGLMatrixName("modelMat", "txtureMat");
        }
        if (mBaseView instanceof BaseGLESCameraSurfaceView) {
            ((BaseGLESCameraSurfaceView) mBaseView).setShaderName(curShader[0], curShader[1], curShader[2]);
            ((BaseGLESCameraSurfaceView) mBaseView).setEGLMatrixName("modelMat", "txtureMat");
        }

        findViewById(R.id.captureBtn).setOnClickListener(v->capture());
        findViewById(R.id.switchCameraBtn).setOnClickListener(v->mCameraManager.switchCamera());

        if(mBaseView instanceof BaseGLESCameraTextureView || mBaseView instanceof BaseGLESCameraSurfaceView)
            findViewById(R.id.switchFilter).setOnClickListener(v->changeEffect());

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
    }

    /**
     * {@inheritDoc}
     * <p>
     * Dispatch onPause() to fragments.
     */
    @Override
    protected void onPause() {
        super.onPause();
        ((BaseView)mBaseView).Pause();
    }

    /**
     * {@inheritDoc}
     * <p>
     * Dispatch onResume() to fragments.  Note that for better inter-operation
     * with older versions of the platform, at the point of this call the
     * fragments attached to the activity are <em>not</em> resumed.
     */
    @Override
    protected void onResume() {
        super.onResume();
        ((BaseView)mBaseView).Resume();
    }

    /**
     *
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();
        ((BaseView)mBaseView).Close();
    }

    /**
     * @param newConfig
     */
    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        // 检查新的屏幕方向
        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            // 横屏模式下的布局
            mBaseView.setLayoutParams(new FrameLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.MATCH_PARENT
            ));
        } else if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT) {
            // 竖屏模式下的布局
            mBaseView.setLayoutParams(new FrameLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT
            ));
        }
    }

    void capture() {
        mCameraManager.takePicture(new SavePictureCallback());
    }

    void changeEffect() {
        mShaderIndex = (++mShaderIndex) % mShaderList.size();
        String[] curShader = mShaderList.get(mShaderIndex);
        if(mBaseView instanceof BaseGLESCameraTextureView) {
            ((BaseGLESCameraTextureView) mBaseView).setShaderName(curShader[0], curShader[1], curShader[2]);
            ((BaseGLESCameraTextureView) mBaseView).setEGLMatrixName("modelMat", "txtureMat");
        }
        if (mBaseView instanceof BaseGLESCameraSurfaceView) {
            ((BaseGLESCameraSurfaceView) mBaseView).setShaderName(curShader[0], curShader[1], curShader[2]);
            ((BaseGLESCameraSurfaceView) mBaseView).setEGLMatrixName("modelMat", "txtureMat");
        }
    }
}
