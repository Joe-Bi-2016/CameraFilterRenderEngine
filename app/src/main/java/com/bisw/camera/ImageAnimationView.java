package com.bisw.camera;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import com.bisw.camera.base.Utils;
import com.bisw.filterRenderEngine.NativeGLRender;
import java.nio.ByteBuffer;
import java.util.HashMap;

public class ImageAnimationView extends AppCompatActivity {
    private final String TAG = ImageAnimationView.class.getSimpleName();
    private ImageView mImageView = null;
    private Button mBtnRest = null, mBtnBk = null, mBtnSwitch = null;
    private final int RESERIMGAGEVIEW = 100;
    private Handler mMainHandler = null;
    private RenderThread mRenderThread;
    private HashMap<Integer, String[]> mShaderList;
    private HashMap<String, UniformParameter[]> mShaderUniformParameter;
    private int mShaderIndex = 0;
    private float step = 0.018f;
    private UniformParameter[] mCurrentUniformParam;
    private long mCurrentTime = 0L;
    private float mQuantLevel = 2.0f;
    private float mWaterPower = 8.0f;
    private boolean mFirstTime = true;
    private Object mStartLock = new Object();
    private boolean mReady = false;

    private class UniformParameter{
        public String uniformName;
        public float value;

        UniformParameter(String name, float val) {
            uniformName = name; value = val;
        }
    }

    @Override public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_imageanimation);

        mImageView = (ImageView) findViewById(R.id.imageView);
        mMainHandler = new Handler(Looper.myLooper()){
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);
                if (msg.what == RESERIMGAGEVIEW) {
                    mImageView.setImageResource(R.drawable.leg);
                } else {
                    Bitmap bitmap = (Bitmap) msg.obj;
                    if (bitmap != null)
                        mImageView.setImageBitmap(bitmap);
                    if(mRenderThread != null) {
                        RenderHandler handler = mRenderThread.getRenderHandler();
                        if (mShaderIndex == 1) { // Marquee
                            // calculate uniform value and set it
                            if (mCurrentUniformParam != null) {
                                for (int i = 0; i < mCurrentUniformParam.length; ++i) {
                                    mCurrentUniformParam[i].value += step;
                                    if (mCurrentUniformParam[i].value >= (2.0f + step))
                                        mCurrentUniformParam[i].value -= 2.0f;
                                    handler.setUnifromParam(mCurrentUniformParam[i].uniformName, mCurrentUniformParam[i].value);
                                }
                            }

                            handler.drawFrame();
                        } else if (mShaderIndex == 3) { // Heart
                            if (mCurrentUniformParam != null) {
                                for (int i = 0; i < mCurrentUniformParam.length; ++i) {
                                    if (mCurrentTime == 0)
                                        mCurrentTime = System.currentTimeMillis();
                                    long t = System.currentTimeMillis() - mCurrentTime;
                                    float vt = t / 1000.0f;
                                    if (vt > 2.0f)
                                        mCurrentTime = 0;
                                    mCurrentUniformParam[i].value = vt;
                                    handler.setUnifromParam(mCurrentUniformParam[i].uniformName, mCurrentUniformParam[i].value);
                                }
                            }

                            handler.drawFrame();
                        }
                    }
                }
            }
        };

        mShaderList = new HashMap<Integer, String[]>();
        String[] filterName = {"3DLUT", "Marquee", "Dip", "Heart", "Noise"};
        mShaderList.put(0, new String[]{filterName[0], "imagevert.glsl", "image_lutfrag.glsl"});
        mShaderList.put(1, new String[]{filterName[1], "imagevert.glsl", "image_animationfrag.glsl"});
        mShaderList.put(2, new String[]{filterName[2], "imagevert.glsl", "image_dipfrag.glsl"});
        mShaderList.put(3, new String[]{filterName[3], "imagevert.glsl", "image_heartfrag.glsl"});
        mShaderList.put(4, new String[]{filterName[4], "imagevert.glsl", "image_noisefrag.glsl"});

        mShaderUniformParameter = new HashMap<String, UniformParameter[]>();
        mShaderUniformParameter.put(filterName[0], new UniformParameter[]{
                new UniformParameter("intensity", 0.0f) });
        mShaderUniformParameter.put(filterName[1], new UniformParameter[]{
                new UniformParameter("ratio", 0.0f) });
        mShaderUniformParameter.put(filterName[2], null);
        mShaderUniformParameter.put(filterName[3], new UniformParameter[]{
                new UniformParameter("u_time", 0.0f) });
        mShaderUniformParameter.put(filterName[4], new UniformParameter[]{
                new UniformParameter("uQuantLevel", 0.0f),
                new UniformParameter("uWaterPower", 0.0f)});

        mCurrentUniformParam = mShaderUniformParameter.get(filterName[mShaderIndex]);

        mRenderThread = new RenderThread(this, mMainHandler);
        mRenderThread.start();

        mBtnRest = (Button) findViewById(R.id.buttonReset);
        mBtnBk = (Button) findViewById(R.id.buttonBk);
        mBtnSwitch = (Button) findViewById(R.id.buttonSwitch);
        mBtnSwitch.setText(filterName[mShaderIndex]);

        mBtnRest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mRenderThread != null) {
                    RenderHandler handler = mRenderThread.getRenderHandler();
                    handler.setStopRender();

                    synchronized (mStartLock) {
                        while (!mReady) {
                            try {
                                mStartLock.wait();
                            } catch (InterruptedException e) {
                                Log.e(TAG, e.getMessage());
                            }
                        }
                        mReady = false;
                    }

                    if (mCurrentUniformParam != null) {
                        for (int i = 0; i < mCurrentUniformParam.length; ++i)
                            mCurrentUniformParam[i].value = 0.0f;
                    }
                    mCurrentUniformParam = mShaderUniformParameter.get(filterName[mShaderIndex]);
                    mCurrentTime = 0;

                    mMainHandler.sendMessage(mMainHandler.obtainMessage(RESERIMGAGEVIEW));
                }
            }
        });

        mBtnBk.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mRenderThread != null) {
                    RenderHandler handler = mRenderThread.getRenderHandler();
                    // set starting render, otherwise the parameter could not been set
                    handler.setStartRender();
                    // load shader
                    handler.loadShader();

                    if (mShaderIndex == 0) { // 3DLUT
                        // load resource
                        handler.setResource(new int[]{R.drawable.leg, R.drawable.mellow});
                        // set viewport
                        handler.setViewPort(933, 1400);
                        // calculate uniform value and set it
                        if (mCurrentUniformParam != null) {
                            for (int i = 0; i < mCurrentUniformParam.length; ++i) {
                                mCurrentUniformParam[i].value += 0.1f;
                                mCurrentUniformParam[i].value = mCurrentUniformParam[i].value > 1.0f ? 0.0f : mCurrentUniformParam[i].value;
                                handler.setUnifromParam(mCurrentUniformParam[i].uniformName, mCurrentUniformParam[i].value);
                            }
                        }
                    } else if (mShaderIndex == 1) { // Marquee
                        // load resource
                        handler.setResource(new int[]{R.drawable.leg, R.drawable.lye});
                        // set viewport
                        handler.setViewPort(mImageView.getWidth(), mImageView.getHeight());
                        // calculate uniform value and set it
                        if (mCurrentUniformParam != null) {
                            for (int i = 0; i < mCurrentUniformParam.length; ++i) {
                                handler.setUnifromParam(mCurrentUniformParam[i].uniformName, mCurrentUniformParam[i].value);
                            }
                        }
                    } else if (mShaderIndex == 2) { // Dip
                        // load resource
                        handler.setResource(new int[]{R.drawable.lye});
                        // set viewport
                        handler.setViewPort(mImageView.getWidth(), mImageView.getHeight());
                        // calculate uniform value and set it
                        if (mCurrentUniformParam != null) {
                            for (int i = 0; i < mCurrentUniformParam.length; ++i) {
                                handler.setUnifromParam(mCurrentUniformParam[i].uniformName, mCurrentUniformParam[i].value);
                            }
                        }
                    }else if (mShaderIndex == 3) { // Heart
                        // no resource
                        // set viewport
                        handler.setViewPort(mImageView.getWidth(), mImageView.getHeight());
                        // calculate uniform value and set it
                        if (mCurrentUniformParam != null) {
                            for (int i = 0; i < mCurrentUniformParam.length; ++i) {
                                handler.setUnifromParam(mCurrentUniformParam[i].uniformName, mCurrentUniformParam[i].value);
                            }
                        }
                    }else if (mShaderIndex == 4) { // Noise
                        // load resource
                        handler.setResource(new int[]{R.drawable.leg, R.drawable.mellow});
                        // set viewport
                        handler.setViewPort(mImageView.getWidth(), mImageView.getHeight());
                        if (mCurrentUniformParam != null) {
                            // calculate uniform value and set it
                            mCurrentUniformParam[0].value += 0.5f;
                            if (mCurrentUniformParam[0].value > 6.0f)
                                mCurrentUniformParam[0].value = mQuantLevel;
                            mCurrentUniformParam[1].value += 0.5f;
                            if (mCurrentUniformParam[1].value > 64.0f)
                                mCurrentUniformParam[1].value = mWaterPower;
                            for (int i = 0; i < mCurrentUniformParam.length; ++i) {
                                handler.setUnifromParam(mCurrentUniformParam[i].uniformName, mCurrentUniformParam[i].value);
                            }
                        }
                    }

                    // draw
                    handler.drawFrame();
                }
            }
        });

        mBtnSwitch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mRenderThread != null) {
                    RenderHandler handler = mRenderThread.getRenderHandler();
                    handler.setStopRender();

                    synchronized (mStartLock) {
                        while (!mReady) {
                            try {
                                mStartLock.wait();
                            } catch (InterruptedException e) {
                                Log.e(TAG, e.getMessage());
                            }
                        }
                        mReady = false;
                    }

                    mShaderIndex = ++mShaderIndex % mShaderList.size();
                    mBtnSwitch.setText(filterName[mShaderIndex]);
                    if (mCurrentUniformParam != null) {
                        for (int i = 0; i < mCurrentUniformParam.length; ++i)
                            mCurrentUniformParam[i].value = 0.0f;
                    }
                    mCurrentUniformParam = mShaderUniformParameter.get(filterName[mShaderIndex]);
                    mCurrentTime = 0;

                    handler.addRenderPass(mShaderIndex);

                    mMainHandler.sendMessage(mMainHandler.obtainMessage(RESERIMGAGEVIEW));
                }
            }
        });
    }

    /**
     *
     */
    @Override
    protected void onResume() {
        super.onResume();
    }

    /**
     * {@inheritDoc}
     * <p>
     * Dispatch onPause() to fragments.
     */
    @Override
    protected void onPause() {
        super.onPause();
    }

    /**
     *
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(mRenderThread != null) {
            RenderHandler handler = mRenderThread.getRenderHandler();
            handler.exitRenderThread();
        }
    }

    private class RenderThread extends Thread {
        private AssetManager mAssetManager;
        private NativeGLRender mNativeRender;
        private Context mContext;
        private Handler mMainHandler;
        private RenderHandler mSelfHandler;
        private int mReadWidth, mReadHeight;
        private HashMap<Integer, Integer> mFilterResourceStatus;
        private final  int SHADERLOADED = 1 << 0;
        private final  int RESOURCELOADED = 1 << 1;
        private boolean mStopRendering = true;


        RenderThread(Context context, Handler handler) {
            mContext = context;
            mMainHandler = handler;
            mAssetManager = context.getAssets();
            mNativeRender = new NativeGLRender();
            mShaderIndex = 0;
            mReadWidth = mReadHeight = 0;
            mFilterResourceStatus = new HashMap<Integer, Integer>();
        }

        public RenderHandler getRenderHandler() {
            return mSelfHandler;
        }

        /**
         *
         */
        @Override
        public void run() {
            super.run();

            Looper.prepare();

            mSelfHandler = new RenderHandler(this);

            // initialize offscreen EGL
            mNativeRender.native_init();
            // set assetManger to GLES context
            mNativeRender.native_setAssetsManger(mAssetManager);

            Looper.loop();

            mNativeRender.native_release();
        }

        public void addRenderPass(int renderpassIndex) {
            mNativeRender.native_addRenderPass(renderpassIndex);
        }
        public void loadShader() {
            // load shader
            if (mFilterResourceStatus.isEmpty() ||
                mFilterResourceStatus.get(mShaderIndex) == null ||
                (mFilterResourceStatus.get(mShaderIndex) & SHADERLOADED) != SHADERLOADED) {
                String[] curShader = mShaderList.get(mShaderIndex);
                mNativeRender.native_addRenderPass(mShaderIndex);
                mNativeRender.native_setProgramShaderFromAssets(mShaderIndex, curShader[0], curShader[1], curShader[2]);
                if (mFilterResourceStatus.isEmpty() || mFilterResourceStatus.get(mShaderIndex) == null)
                    mFilterResourceStatus.put(mShaderIndex, SHADERLOADED);
                else {
                    int val = mFilterResourceStatus.get(mShaderIndex);
                    mFilterResourceStatus.put(mShaderIndex, val | SHADERLOADED);
                }
            }
        }

        // load resource
        public void loadResource(int[] resIdList) {
            if (mFilterResourceStatus.isEmpty() ||
                mFilterResourceStatus.get(mShaderIndex) == null ||
                (mFilterResourceStatus.get(mShaderIndex) & RESOURCELOADED) != RESOURCELOADED) {
                for (int i = 0; i < resIdList.length; ++i) {
                    Bitmap bitmap = Utils.loadRGBAImage(mContext, resIdList[i]);
                    if (bitmap != null)
                    {
                        int bytes = bitmap.getByteCount();
                        ByteBuffer buf = ByteBuffer.allocate(bytes);
                        bitmap.copyPixelsToBuffer(buf);
                        byte[] byteArray = buf.array();
                        mNativeRender.native_setTexture(NativeGLRender.RGBA8888, bitmap.getWidth(), bitmap.getHeight(), byteArray, mShaderIndex, i);
                    }
                }

                if (mFilterResourceStatus.isEmpty() || mFilterResourceStatus.get(mShaderIndex) == null)
                    mFilterResourceStatus.put(mShaderIndex, RESOURCELOADED);
                else {
                }
                    int val = mFilterResourceStatus.get(mShaderIndex);
                    mFilterResourceStatus.put(mShaderIndex, val | RESOURCELOADED);
            }

        }

        // set parameter
        public void setParameter(String uniformName, float intensity) {
            if (mStopRendering)
                return;

            mNativeRender.native_setParamFloat(mShaderIndex, uniformName, intensity);
        }

        public void setViewPort(int width, int height) {
            if (mStopRendering)
                return;

            if (mReadWidth != width || mReadHeight != height) {
                mNativeRender.native_onSurfaceChanged(width, height);
                mReadWidth = width;
                mReadHeight = height;
            }
        }
        public void drawFrame() {
            if (mStopRendering)
                return;

            if (mFirstTime) {
                // the first frame is empty, so should render two time
                mNativeRender.native_onDrawFrame(mShaderIndex);
                mFirstTime = false;
            }

            mNativeRender.native_onDrawFrame(mShaderIndex);

            // read render result of pixel from GL
            Bitmap bitmap = mNativeRender.native_readPixel(0, 0, mReadWidth, mReadHeight);
            // send bitmap to main thread for updating imageview
            Message msg = mMainHandler.obtainMessage();
            msg.obj = bitmap;
            mMainHandler.sendMessage(msg);
        }

        public void stopRendering() {
            synchronized (mStartLock) {
                mStopRendering = true;
                mReady = true;
                mStartLock.notify();
            }
        }

        public void startRendering() {
            mStopRendering = false;
        }

        public void exit() {
            Looper.myLooper().quit();
        }

    }
    private class RenderHandler extends Handler {
        private final int MSG_ADDRENDERPASS = 0;
        private final int MSG_LOADSHADER    = 1;
        private final int MSG_LOADRESOURCE  = 2;
        private final int MSG_SETVIEWPORT   = 3;
        private final int MSG_SETPARAMETER  = 4;
        private final int MSG_DRAWFRAME     = 5;
        private final int MSG_STARTRENDER   = 6;
        private final int MSG_STOPRENDER    = 7;
        private final int MSG_EXIT          = 8;
        private RenderThread mRenderThread;
        RenderHandler(RenderThread thread) {
            mRenderThread = thread;
        }

        public void addRenderPass(int renderpassIdx) {
            sendMessage(obtainMessage(MSG_ADDRENDERPASS, renderpassIdx, 0, null));
        }
        public void loadShader() {
            sendMessage(obtainMessage(MSG_LOADSHADER));
        }

        public void setResource(int[] resource) {
            sendMessage(obtainMessage(MSG_LOADRESOURCE, 0, 0, resource));
        }

        public void setViewPort(int width, int height) {
            sendMessage(obtainMessage(MSG_SETVIEWPORT, width, height, null));
        }
        public void setUnifromParam(String uniformName, float val) {
            Bundle b = new Bundle();
            b.putString("uniformName", uniformName);
            b.putFloat("uniformValue", val);
            Message msg = obtainMessage(MSG_SETPARAMETER);
            msg.setData(b);
            sendMessage(msg);
        }

        public void drawFrame() {
            sendMessage(obtainMessage(MSG_DRAWFRAME));
        }

        public void setStartRender() {
            sendMessage(obtainMessage(MSG_STARTRENDER));
        }

        public void setStopRender() {
            sendMessage(obtainMessage(MSG_STOPRENDER));
        }

        public void exitRenderThread() {
            sendMessage(obtainMessage(MSG_EXIT));
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_ADDRENDERPASS:
                    mRenderThread.addRenderPass(msg.arg1);
                    break;
                case MSG_LOADSHADER:
                    mRenderThread.loadShader();
                    break;
                case MSG_LOADRESOURCE:
                {
                    int[] resIdceList = (int[]) msg.obj;
                    mRenderThread.loadResource(resIdceList);
                }
                    break;
                case MSG_SETVIEWPORT:
                    mRenderThread.setViewPort(msg.arg1, msg.arg2);
                    break;
                case MSG_SETPARAMETER: {
                    Bundle b = msg.getData();
                    String uniformName = b.getString("uniformName");
                    float val = b.getFloat("uniformValue");
                    mRenderThread.setParameter(uniformName, val);
                }
                    break;
                case MSG_DRAWFRAME:
                    mRenderThread.drawFrame();
                    break;
                case MSG_STARTRENDER:
                    mRenderThread.startRendering();
                    break;
                case MSG_STOPRENDER:
                    mRenderThread.stopRendering();
                    break;
                case MSG_EXIT:
                    mRenderThread.exit();
                    break;
            }
        }
    }

}
