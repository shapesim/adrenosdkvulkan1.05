/*
 * Model Fragment shader 
 */
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 1) uniform sampler2D tex;

layout (location = 0) in vec4 color;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 pos;
layout (location = 3) in vec3 viewDir;
layout (location = 4) in vec3 normal;

#define saturate(x) clamp( x, 0.0, 1.0 )

layout (location = 0) out vec4 FragColor;

void main() {

  vec4 Llightpos = vec4(-60, -25, -3, 0.0f);
  vec4 Ldiffuse = vec4(0.5f, 0.5f, 0.4f, 1.0f);
  
  vec4 Llightpos2 = vec4(15, -10, 3, 0.0f);
  
  vec3 vNormal = normalize(-normal);
  
  vec3 diffuse =  Ldiffuse.xyz;

  vec3 vLightPos =  Llightpos.xyz - pos.xyz;
  vec3 vLightPosDir = normalize (vLightPos);
  
  vec3 vLightPos2 =  Llightpos2.xyz - pos.xyz;
  vec3 vLightPosDir2 = normalize (vLightPos2);
  
  float intensity = max(dot(vNormal,vLightPosDir), 0.3);
  intensity += max(dot(vNormal,vLightPosDir2), 0.3);
 
  vec3 texCol = texture(tex, uv).xyz;
 
  FragColor = vec4(intensity*texCol, 1.0f);
}
