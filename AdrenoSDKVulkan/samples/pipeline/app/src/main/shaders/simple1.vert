// Vertex shadex
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set = 0, binding = 0) uniform all {
                                                mat4 mvp;
                                                mat4 m;
                                                mat4 v;
                                                vec3 camPos;
                                              } data;

layout (location = 0) in vec4 vertPos;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec2 vertUV;
layout (location = 3) in vec3 vertNormal;

layout (location = 0) out vec4 pos;
layout (location = 1) out vec3 normal;

out gl_PerVertex
{
   vec4 gl_Position;
};

void main()
{
    pos    = vertPos;
    normal = vertNormal;

    gl_Position = data.mvp * (vertPos - vec4(20.0f, 0.0f, 0.0f, 0.0f));
}
