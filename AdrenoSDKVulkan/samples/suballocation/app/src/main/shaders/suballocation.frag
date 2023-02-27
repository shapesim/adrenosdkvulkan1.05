/*
 * Suballocation Fragment shader
 */
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 color;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 pos;
layout (location = 3) in vec3 viewDir;
layout (location = 4) in vec3 normal;

layout(push_constant) uniform lightData {
  vec4 lightpos;
  vec4 diffuse;
} light;

#define saturate(x) clamp( x, 0.0, 1.0 )

layout (location = 0) out vec4 FragColor;

void main() {
  vec3 vNormal = normalize(normal);
  
  vec3 diffuse = light.diffuse.xyz;

  vec3 vLightPos = light.lightpos.xyz - pos.xyz;
  vec3 vLightPosDir = normalize (vLightPos);
  float intensity = max(dot(vNormal,vLightPosDir), 0.1);
  if (intensity > 0.0f)
  {
    diffuse += (intensity + saturate( 1.0 - 0.45 * dot( vLightPos, vLightPos ) ));
  }
 
  FragColor = vec4(color.xyz*diffuse , 1.0f);
}
