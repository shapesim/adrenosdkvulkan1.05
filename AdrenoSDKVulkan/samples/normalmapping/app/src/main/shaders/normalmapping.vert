/*
 * Basic Normalmapping Vertex shader
 */
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (std140, set = 0, binding = 0) uniform all { 
                                                mat4 mvp;
                                                vec4 lightPos;
                                                vec4 camPos;
                                              } data;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 vertColor;
layout (location = 2) in vec2 vertUV;
layout (location = 3) in vec3 verttangent;
layout (location = 4) in vec3 vertbinormal;
layout (location = 5) in vec3 vertNormal;

layout (location = 0) out vec4 color;
layout (location = 1) out vec2 uv;
layout (location = 2) out vec3 lightDir;
layout (location = 3) out vec3 viewDir;
layout (location = 4) out vec3 normal;

out gl_PerVertex
{
   vec4 gl_Position;
};

void main() {
  gl_Position  = data.mvp * vec4(pos, 1.0f);
  //gl_Position  = vec4(pos.x*0.25 * data.mvp[0][0], pos.z*0.25*data.mvp[0][0], 0.0f, 1.0f);

  uv = vertUV;
  color = vertColor;
  normal = vertNormal;

  vec3 lightVec = data.lightPos.xyz - pos.xyz;
  vec3 viewVec  = data.camPos.xyz   - pos.xyz;

  // When using normal maps, transform vectors into tangent space
  lightDir.x = dot( lightVec.xyz, verttangent);
  lightDir.y = dot( lightVec.xyz, vertbinormal);
  lightDir.z = dot( lightVec.xyz, vertNormal);

  viewDir.x  = dot( viewVec.xyz, verttangent );
  viewDir.y  = dot( viewVec.xyz, vertbinormal);
  viewDir.z  = dot( viewVec.xyz, vertNormal );
}
