package com.bisw.filterRenderEngine;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.view.Surface;

public class NativeGLRender {
    /**
     * enum type of image format
     */
    public static final int BGRA8888 = 1;
    public static final int RGBA8888 = 2;
    public static final int RGB888 = 3;
    public static final int RGB565 = 4;
    public static final int A8 = 5;
    public static final int I8 = 6;
    public static final int AI88 = 7;
    public static final int RGBA4444 = 8;
    public static final int RGB5A1 = 9;
    public static final int PVRTC4 = 10;
    public static final int PVRTC4A = 11;
    public static final int PVRTC2 = 12;
    public static final int PVRTC2A = 13;
    public static final int ETC = 14;
    public static final int S3TC_DXT1 = 15;
    public static final int S3TC_DXT3 = 16;
    public static final int S3TC_DXT5 = 17;
    public static final int ATC_RGB = 18;
    public static final int ATC_EXPLICIT_ALPHA = 19;
    public static final int ATC_INTERPOLATED_ALPHA = 20;
    public static final int MTL_ABGR4 = 21;
    public static final int MTL_B5G6R5 = 22;
    public static final int MTL_BGR5A1 = 23;

    /**
     * Initialize filter-renderengine with display surface, the surface could come from surfaceview,
     * or textureview, other surfaces may also be supported. I only test surfaceview and textureview
     *
     * @ param surface : the surface of surfaceview or textureview
     * @ return : None
     */
    public native void native_initWithSurface(Surface surface);

    /**
     * Initialize filter-renderengine with nothing, that will create background rendering context
     *
     * @ param : None
     * @ return : None
     */
    public native void native_init();

    /**
     * destroy EGLsurface
     *
     * @ param : None
     * @ return : None
     */
    public native void native_onDestroySurface();

    /**
     * release filter-renderengine
     *
     * @ param : None
     * @ return : None
     */
    public native void native_release();

    /**
     * setting the renderPass's vertex shader and fragment shader
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param programName: the name of program shader
     * @ param vertShader : the string of vertex shader
     * @ param fragShader : the string of fragment shader
     * @ return : None
     */
    public native void native_setProgramShader(int renderPassIndex, String programName, String vertShader, String fragShader);

    /**
     * setting the AssetsManager to NDK, which will be used to read file form assets folder in ndk.
     *
     * @ param assetManager : the AssetManager of current application
     * @ return : None
     */
    public native void native_setAssetsManger(AssetManager assetManager);

    /**
     * setting the renderPass's vertex shader and fragment shader from assets file folder
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param programName: the name of program shader
     * @ param vertShader : the name of vertex shader in assets
     * @ param fragShader : the name of fragment shader in assets
     * @ return : None
     */
    public native void native_setProgramShaderFromAssets(int renderPassIndex, String programName, String vertShader, String fragShader);

    /**
     * upload image data to the renderPass's texture
     *
     * @ param format : the format of image data, type is upper enum value
     * @ param width : the width of image data
     * @ param height : the height of image data
     * @ param data : the data of image
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param index : the data will be upload to which texture of the renderPass
     * @ return : None
     */
    public native void native_setTexture(int format, int width, int height, byte[] data, int renderPassIndex, int index);

    /**
     * create OEStexture
     *
     * @ param format : the format of image data, type is upper enum value
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param index : the data will be upload to which texture of the renderPass
     * @ return : texture id
     */
    public native int native_createOESTexture(int renderPassIndex, String name);

    /**
     * set all render background color
     *
     * @ param r : the red heft of color, scope is [0.0f ~ 1.0f]
     * @ param g : the green heft of color, scope is [0.0f ~ 1.0f]
     * @ param b : the blue heft of color, scope is [0.0f ~ 1.0f]
     * @ param a : the alpha heft of color, scope is [0.0f ~ 1.0f]
     * @ return : None
     */
    public native void native_setBackgroundColor(float r, float g, float b, float a);

    /**
     * add a render pass
     *
     * @ param renderPassIndex : the index of renderPass, from 1 start
     * @ return : None
     */
    public native void native_addRenderPass(int renderPassIndex); // renderPassIndex from 1 start

    /**
     * set which renderPass's background color
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param r : the red heft of color, scope is [0.0f ~ 1.0f]
     * @ param g : the green heft of color, scope is [0.0f ~ 1.0f]
     * @ param b : the blue heft of color, scope is [0.0f ~ 1.0f]
     * @ param a : the alpha heft of color, scope is [0.0f ~ 1.0f]
     * @ return : None
     */
    public native void native_setRenderPassBackGroundColor(int renderPassIndex, float r, float g, float b, float a);

    /**
     * set the fbo of the fboRenderPassIndex renderPass as input to renderPassIndex renderPass
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param fboRenderPassIndex : the index of renderPass, default index is 0, the renderPass's fbo will be used to renderPassIndex renderPass's
     * input
     * @ return : None
     */
    public native void native_addRenderFboToRenderPass(int renderPassIndex, int fboRenderPassIndex);

    /**
     * set the renderPassIndex renderPass's parameter
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param paramName : the name in shader, the shader had been bind to the renderPassIndex renderPass
     * @ param value : the value of paramName, type is integer
     * @ return : None
     */
    public native void native_setParamInt(int renderPassIndex, String paramName, int value);

    /**
     * set the renderPassIndex renderPass's parameter
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param paramName : the name in shader, the shader had been bind to the renderPassIndex renderPass
     * @ param value : the value of paramName, type is float
     * @ return : None
     */
    public native void native_setParamFloat(int renderPassIndex, String paramName, float value);

    /**
     * set the renderPassIndex renderPass's parameter
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param paramName : the name in shader, the shader had been bind to the renderPassIndex renderPass
     * @ param value : the value of paramName, type is bool
     * @ return : None
     */
    public native void native_setParamBool(int renderPassIndex, String paramName, boolean value);

    /**
     * set the renderPassIndex renderPass's parameter
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param paramName : the name in shader, the shader had been bind to the renderPassIndex renderPass
     * @ param value : the value of paramName, floating-point types vector2, vector3, vector4
     * @ return : None
     */
    public native void native_setParamVector(int renderPassIndex, String paramName, float[] value);

    /**
     * set the renderPassIndex renderPass's parameter
     *
     * @ param renderPassIndex : the index of renderPass, default index is 0
     * @ param paramName : the name in shader, the shader had been bind to the renderPassIndex renderPass
     * @ param value : the value of paramName, floating-point types mat2, mat3, mat4
     * @ return : None
     */
    public native void native_setParamMatrix(int renderPassIndex, String paramName, float[] value);

    /**
     * when surface created to call it, which will set render back color to default white
     *
     * @ param : None
     * @ return: None
     */
    public native void native_onSurfaceCreated();

    /**
     * when surface changes occur to call it, which will set render viewport size
     *
     * @ param width: width of viewport
     * @ param width: height of viewport
     * @ return: None
     */
    public native void native_onSurfaceChanged(int width, int height);

    /**
     * call filter-renderengine to draw frame of which renderPass
     *
     * @ param renderPassIndex : the index of renderPass, index is -1, which represents all pass been rendered
     * @ return : None
     */
    public native void native_onDrawFrame(int renderPassIndex);

    /**
     * get pixels of current frame
     * param x: read start position of screen horizontal coordinates
     * param y: read start position of screen ordinates
     *
     * @ param width: read width
     * @ param width: read height
     * @ return: return a bitmap
     */
    public native Bitmap native_readPixel(int x, int y, int width, int height);

    /**
     * get pixels of current frame
     * param x: read start position of screen horizontal coordinates
     * param y: read start position of screen ordinates
     *
     * @ param width: read width
     * @ param width: read height
     * @ return: bytes of pixels
     */
    public native byte[] native_getPixel(int x, int y, int width, int height);

    // Used to load the 'filterRenderEngine' library on application startup.
    static {
        System.loadLibrary("filterRenderEngine");
    }
}