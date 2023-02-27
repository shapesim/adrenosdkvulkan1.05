// Object Vertex Shader

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set = 0, binding = 0) uniform matrix
{
    mat4 mvp;
    mat4 modelView;
} matrices;


layout (location = 0) in vec3 inPosition;
layout (location = 3) in vec3 inNormal;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outView;
layout (location = 3) out vec3 outLight;
layout (location = 4) out mat4 outInvModelView;

out gl_PerVertex
{
   vec4 gl_Position;
};

void main()
{
    // Compute position of the object
   gl_Position  = matrices.mvp * vec4(inPosition, 1.0f) * vec4(1.0,-1.0,1.0,1.0);

   // Compute values needed for lighting and cubemap sampling in view space
   outPosition = normalize(vec3(matrices.modelView * vec4(inPosition, 1.0)));
   outNormal =  normalize(mat3(matrices.modelView) * inNormal);
   outView = normalize(-outPosition);
   outLight = normalize(vec3(10.0,-10.0,10.0) - outPosition);
   outInvModelView = inverse(matrices.modelView);
}
