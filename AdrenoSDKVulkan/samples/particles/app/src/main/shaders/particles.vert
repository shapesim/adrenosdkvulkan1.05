// Particle Vertex shader
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set = 0, binding = 0) uniform VertexUBO
{
    mat4 mvp;
    mat4 invcamera;
    int  tileRows;
    int  tileCols;
} vertexUBO;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec3 vertUV;

layout (location = 0) out vec4 color;
layout (location = 1) out vec2 uv;

out gl_PerVertex
{
   vec4 gl_Position;
};

void main() {

    float scale = 1.0;
    vec2 offset = scale * (vertUV.xy -0.5);

    // Compute billboard position of particle using inverse camera matrix
    vec4 adjPos = vec4(pos,1.0f) + vertexUBO.invcamera*vec4(1.0f,0.0f,0.0f,0.0f)* offset.x +vertexUBO.invcamera*vec4(0.0f,1.0f,0.0f,0.0f)* offset.y;
    gl_Position  = vertexUBO.mvp * adjPos;

    // Output color
    color = vertColor;

    // Adjust UV for tile position
    float tileXDelta = 1.0f/vertexUBO.tileCols;
    float tileYDelta = 1.0f/vertexUBO.tileRows;
    int tileXOffset = int(vertUV.z) % vertexUBO.tileCols;
    int tileYOffset =  vertexUBO.tileRows - 1 - int(vertUV.z) / vertexUBO.tileRows;

    uv[0] = vertUV[0]*tileXDelta + tileXOffset*tileXDelta;
    uv[1] = vertUV[1]*tileYDelta + tileYOffset*tileYDelta;
}
