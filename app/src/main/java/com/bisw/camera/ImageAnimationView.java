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
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import com.bisw.camera.base.Utils;
import com.bisw.filterRenderEngine.NativeGLRender;
import java.nio.ByteBuffer;

public class ImageAnimationView extends AppCompatActivity {
    private final String TAG = ImageAnimationView.class.getSimpleName();
    private ImageView mImageView = null;
    private Button mBtn = null;
    private Handler mMainHandler = null;
    private RenderThread mRenderThread;
    private float intensity = 0.0f;

    @Override public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_imageanimation);

        mImageView = (ImageView) findViewById(R.id.imageView);
        mMainHandler = new Handler(Looper.myLooper()){
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);
                Bitmap bitmap = (Bitmap) msg.obj;
                mImageView.setImageBitmap(bitmap);
            }
        };

        mRenderThread = new RenderThread(this, mMainHandler);
        mRenderThread.start();

        mBtn = (Button) findViewById(R.id.button);
        mBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mBtn.getText().equals(ImageAnimationView.this.getResources().getString(R.string.btn_txt_reset))) {
                    mImageView.setImageResource(R.drawable.leg);
                    mBtn.setText(R.string.bg_render_txt);
                } else {
                    if(mRenderThread != null) {
                        RenderHandler handler = mRenderThread.getRenderHandler();
                        intensity += 0.1f;
                        intensity = intensity > 1.0f ? 0.0f : intensity;
                        handler.setIntensity(intensity);
                        handler.drawFrame();
                    }
                    mBtn.setText(R.string.btn_txt_reset);
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
        private String mProgramName, mVertName, mFragName;
        private float intensity = 0.0f;

        RenderThread(Context context, Handler handler) {
            mContext = context;
            mMainHandler = handler;
            mAssetManager = context.getAssets();
            mNativeRender = new NativeGLRender();
            mProgramName = "NormalFilter";
            mVertName = "Image_0_vert.glsl";
            mFragName = "Image_0_frag.glsl";
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
            // load shader
            mNativeRender.native_setProgramShaderFromAssets(0, mProgramName, mVertName, mFragName);
            // load resource
            loadResource(mContext, mNativeRender);

            Looper.loop();

            mNativeRender.native_release();
        }

        private void loadResource(Context context, NativeGLRender nativeGLRender) {
            Bitmap bitmap = Utils.loadRGBAImage(context, R.drawable.leg);
            if (bitmap != null)
            {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                nativeGLRender.native_setTexture(NativeGLRender.RGBA8888, bitmap.getWidth(), bitmap.getHeight(), byteArray, 0, 0);
                nativeGLRender.native_onSurfaceChanged(bitmap.getWidth(), bitmap.getHeight());
            }

            bitmap = Utils.loadRGBAImage(context, R.drawable.mellow);
            if (bitmap != null)
            {
                int bytes = bitmap.getByteCount();
                ByteBuffer buf = ByteBuffer.allocate(bytes);
                bitmap.copyPixelsToBuffer(buf);
                byte[] byteArray = buf.array();
                nativeGLRender.native_setTexture(NativeGLRender.RGBA8888, bitmap.getWidth(), bitmap.getHeight(), byteArray, 0, 1);
            }
        }

        public void setParameter(float intensity) {
            // set parameter
            mNativeRender.native_setParamFloat(0, "intensity", intensity);
        }

        public void drawFrame() {
            mNativeRender.native_onDrawFrame(0);
            // read render result of pixel from GL
            Bitmap bitmap = mNativeRender.native_readPixel(0, 0, 933, 1400);
            // send bitmap to main thread for updating imageview
            Message msg = mMainHandler.obtainMessage();
            msg.obj = bitmap;
            mMainHandler.sendMessage(msg);
        }

        public void exit() {
            Looper.myLooper().quit();
        }

    }
    private class RenderHandler extends Handler {
        private final int MSG_SETINTENSITY = 0;
        private final int MSG_DRAWFRAME = 1;
        private final int MSG_EXIT = 2;
        private RenderThread mRenderThread;
        RenderHandler(RenderThread thread) {
            mRenderThread = thread;
        }

        public void setIntensity(float val) {
            Bundle b = new Bundle();
            b.putFloat("intensity", val);
            Message msg = obtainMessage(MSG_SETINTENSITY);
            msg.setData(b);
            sendMessage(msg);
        }

        public void drawFrame() {
            sendMessage(obtainMessage(MSG_DRAWFRAME));
        }

        public void exitRenderThread() {
            sendMessage(obtainMessage(MSG_EXIT));
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SETINTENSITY: {
                    Bundle b = msg.getData();
                    float val = b.getFloat("intensity");
                    mRenderThread.setParameter(val);
                }
                    break;
                case MSG_DRAWFRAME:
                    mRenderThread.drawFrame();
                    break;
                case MSG_EXIT:
                    mRenderThread.exit();
                    break;
            }
        }
    }

}
