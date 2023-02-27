/*
 * vertex shader
 */
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set = 0, binding = 0) uniform all { 
                                                mat4 mvp;
                                                mat4 m;
                                                mat4 v;
                                                vec3 camPos;
                                              } data;

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec2 vertUV;
layout (location = 3) in vec3 vertNormal;
layout (location = 4) in vec3 vertbinormal;
layout (location = 5) in vec3 verttangent;

layout (location = 0) out vec4 color;
layout (location = 1) out vec2 uv;
layout (location = 2) out vec4 pos;
layout (location = 3) out vec3 viewDir;
layout (location = 4) out vec3 normal;

out gl_PerVertex
{
   vec4 gl_Position;
};

void main() {
  
  pos = data.m * vec4(vertPos, 1.0f);
  
  color = vertColor;
  
  viewDir = data.camPos;
  
  gl_Position  = data.mvp * vec4(vertPos, 1.0f);
  
  normal = (data.m * vec4(vertNormal, 1.0f)).xyz;
  
  uv = vertUV;
}
