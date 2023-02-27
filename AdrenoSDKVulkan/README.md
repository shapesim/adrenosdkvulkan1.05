Qualcomm® Adreno™ SDK for Vulkan™
=================================

Release History
V1.05 - Feb 22, 2017
+ Added Gui sample which shows how to add an immediate mode Gui in Vulkan
+ Added Cubemap sample which shows how to sample a cubemap
+ Updated framework files to use KTX file format.
+ Modified several samples to use KTX file format instead of TGA
+ Added center option to mesh object loader to offset objects to origin
+ Added depth stencil state parameter to InitPipeline utility function
+ Updated support for Android Studio 2.2.3, Android NDK r13b, Gradle plugin 0.8.3, and Gradle 2.14.1
+ Verified on Android N

V1.04 - Nov 10, 2016
+ Added Pipeline sample which shows how to derive and cache pipelines
+ Added Multithreading sample which shows how to use secondary command buffers to render different objects in the scene
+ Added Particle sample which shows how to create and update particles rendered as animated screen facing alpha blended quads.
+ Moved framework files to a common folder so that all samples can share the same framework files.
+ Updated support for Android Studio 2.2, Android NDK 13.0, Gradle plugin 0.8.2, and Gradle 2.14.1
+ Verified on Android N

V1.03 – Sept 6, 2016
+ Added Validation Layer support to all samples both in the framework code and non-framework implemented samples
+ Added Validation sample to demonstrate how validation can be enabled.
Added Memory Allocator feature to the framework which uses a simple memory sub-allocation technique.
+ Added Suballocation sample which uses the framework’s memory allocation system when loading a set of objects.
+ Numerous changes to clear validation errors.
+ Centralized the logic in the framework for changing the image layout
+ Modified the loading process for tga and astc files to use vkCopyImage and appropriate image layouts.
+ Code cleanup
+ Verified on both Android M and N

V1.02 – July 11, 2016
+ Updated Vulkan libraries
+ Updated Vulkan include files (changed VK_API_VERSION to VK_API_VERSION_1_0)
+ Updated support for Android Studio 2.1.2
+ Deferred Multipass cleanup and fixes for correct second subpass
+ Verified on Android N Preview

V1.01 – May 5, 2016
+ Added 64bit Vulkan library (libVulkan.so)
+ Updated support for Android Studio 2.0
+ Added deferred multipass sample

V1.0 – March 14, 2016
+ Initial Release


================================================================================
OpenGL Mathematics (GLM)
--------------------------------------------------------------------------------

================================================================================
The MIT License
--------------------------------------------------------------------------------
Copyright (c) 2005 - 2015 G-Truc Creation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

TinyObjLoader.h:
//
// Copyright 2012-2015, Syoyo Fujita.
//
// Licensed under 2-clause BSD license.
//


================================================================================
ImGUI
--------------------------------------------------------------------------------

The MIT License (MIT)

Copyright (c) 2014-2015 Omar Cornut and ImGui contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
