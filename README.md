![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Android%20%7C%20Linux-brightgreen)
![Language](https://img.shields.io/badge/language-C%2B%2B%20%7C%20GLSL-orange)

## Description
> A high-performance, extensible OpenGL ES rendering engine for mobile platforms, enabling unified rendering across multiple scenarios such as camera filters, photo editing, video playback, and non-linear editing.

## 🔥 Key Features

- ✅ **Unified Architecture for Multiple Use Cases**: A single engine supports camera preview, image editing, video processing, and non-linear composition.
- ✅ **Highly Decoupled Design**
  - Separation between rendering and display layers (supports SurfaceView / TextureView / ImageView, etc.)
  - Decoupling of algorithm logic from the rendering engine (supports dynamic loading of GLSL scripts)
- ✅ **Dynamic Filter System**: Hot-reload `.glsl` files to deploy new visual effects without app updates.
- ✅ **Future Roadmap**: Support ShaderToy fragment shaders for rapid integration of a vast library of visual effects.

## 🚀 Application Scenarios
| Scenario | Advantages |
|--------|-----------|
| Camera Apps | Real-time filters with low-latency rendering |
| Photo Editing | Multi-layer compositing and undo/redo support |
| Video Playback | Efficient YUV → RGB conversion with post-processing |
| Non-Linear Editing | Multi-track compositing and transition effects |

## 📦 Integration
Supports CMake-based builds and can be integrated as a native library into Android projects.

## 🤝 Contributions & Feedback
Issues and PRs are welcome! We especially appreciate real-world usage feedback and suggestions.

## 📜 License
MIT LicenseThis project is licensed under the MIT License - see the [LICENSE](LICENSE.txt) file for details.
## CameraFilterRenderEngine

This is an Android camera-related project that demonstrates how to use the Android Camera1 API and Camera2 API with TextureView and SurfaceView for previewing, capturing, and saving photos. Furthermore, the project includes the design and development of a filter rendering engine capable of processing camera effects and applying special effects to other components such as ImageView. This engine is versatile and can be utilized in both camera apps and photo album apps. The filter engine effectively decouples the effect display components from the algorithms, allowing application developers and algorithm specialists to focus on their respective domains. In future phases, a configuration file function can be added to dynamically download special effect algorithms in script form as needed, and it can also be compatible with algorithms from [shaderToy](https://www.shadertoy.com/) web.

## FilterRenderEngine function description

1. Decoupling of display components from the rendering engine and configurable resources, such as camera resources and video playback;
2. Support for various texture formats, including compressed textures: PVRTC, ETC, S3TC, ATC, etc.;
3. Support for EGL Image textures with high performance;
4. Support for rendering and display via SurfaceView, GLSurfaceView, and TextureView;
5. Support for rendering and editing of Camera;
6. Support for 3D animation rendering and interaction within ImageView;
7. Support for multi-render pass renderpasses, enabling nonlinear editing;
8. Support for multimedia playback;
9. Support for importing [shaderToy](https://www.shadertoy.com/)'s shader scripts, facilitating easy porting and integration.




