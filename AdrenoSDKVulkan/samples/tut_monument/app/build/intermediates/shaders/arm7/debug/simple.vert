/*
 * Vertex shader
 */
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec2 vertUV;

layout (location = 0) out vec4 color;
layout (location = 1) out vec2 uv;


layout (std140, set = 0, binding = 0) uniform all { 
                                                mat4 mvp;
                                              } data;

out gl_PerVertex
{
   vec4 gl_Position;
};

void main() {
   color = vertColor;
   uv = vertUV;
   gl_Position = data.mvp*pos;
}
