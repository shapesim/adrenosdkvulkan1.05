// Floor Vertex Shader
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set = 0, binding = 0) uniform VertexUBO
{
    mat4 mvp;
    mat4 camera;
    mat4 invcamera;
    int  tileRows;
    int  tileCols;
} vertexUBO;


layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec2 vertUV;

layout (location = 0) out vec4 color;
layout (location = 1) out vec2 uv;

out gl_PerVertex
{
   vec4 gl_Position;
};

void main() {
   uv = vertUV;
   color = vertColor;
   gl_Position  = vertexUBO.mvp * vec4(pos, 1.0f);
}
