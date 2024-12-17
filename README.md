# CameraFilterRenderEngine
This is an Android camera-related project that demonstrates how to use the Android Camera1 API and Camera2 API with TextureView and SurfaceView for previewing, capturing, and saving photos. Furthermore, the project includes the design and development of a filter rendering engine capable of processing camera effects and applying special effects to other components such as ImageView. This engine is versatile and can be utilized in both camera apps and photo album apps. The filter engine effectively decouples the effect display components from the algorithms, allowing application developers and algorithm specialists to focus on their respective domains. In future phases, a configuration file function can be added to dynamically download special effect algorithms in script form as needed, and it can also be compatible with algorithms from [shaderToy](https://www.shadertoy.com/) web.

## FilterRenderEngine function Description

1. Decoupling of display components from the rendering engine and configurable resources, such as camera resources and video playback;
2. Support for various texture formats, including compressed textures: PVRTC, ETC, S3TC, ATC, etc.;
3. Support for EGL Image textures with high performance;
4. Support for rendering and display via SurfaceView, GLSurfaceView, and TextureView;
5. Support for rendering and editing of Camera;
6. Support for 3D animation rendering and interaction within ImageView;
7. Support for multi-render pass renderpasses, enabling nonlinear editing;
8. Support for multimedia playback;
9. Support for importing [shaderToy](https://www.shadertoy.com/)'s shader scripts, facilitating easy porting and integration.
