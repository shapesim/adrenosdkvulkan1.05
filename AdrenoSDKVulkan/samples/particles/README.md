Particles
===============
This Vulkan example shows one way to calculate and render particles.  The particle is rendered using a texture mapped screen facing quad.  The texture contains several animation tiles which are cycled through.  Every frame the particles position and tile are computed and they are sorted based on their distance from the camera to allow for correct alpha blending.

Prerequisites
-------------
- Tested with Android Studio 2.2.3 with NDK bundle r13b
- Qualcomm? Adreno? SDK for Vulkan?

Getting Started
---------------
1. Launch Android Studio.
2. Open the sample directory.
3. Rebuild the project, which will allow the sample to pick up SDK/NDK locations locally.
4. Click Run/Run 'app'.

Shaders are within app/src/main/jni/shaders. They are automatically built in the Android Studio project, as long as glslangValidator is on the path. More information on shader compilation be found in the "Compiling Shaders to SPIR-V" document.

Debugging
---------
To debug, use the 'app-native' configuration, selecting Debug rather than Run. It is normal for deployment and
application initialization to take significantly more time.

Screenshots
-----------
![screenshot](screenshot.png)
