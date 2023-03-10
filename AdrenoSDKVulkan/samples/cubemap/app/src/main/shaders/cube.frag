// Cube Fragment Shader

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 1) uniform samplerCube cubeTexture;

layout (location = 0) in vec3 inUVW;
layout (location = 0) out vec4 uFragColor;

void main()
{

  uFragColor = texture(cubeTexture, inUVW);
}
