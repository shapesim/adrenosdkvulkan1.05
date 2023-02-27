// Cube Vertex Shader

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set = 0, binding = 0) uniform matrix
{
    mat4 mvp;
} matrices;

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec3 outUVW;

out gl_PerVertex
{
   vec4 gl_Position;
};

void main() {

   outUVW = inPosition * vec3( 1, -1, 1);

   gl_Position  = matrices.mvp * vec4(inPosition, 1.0f);
}
