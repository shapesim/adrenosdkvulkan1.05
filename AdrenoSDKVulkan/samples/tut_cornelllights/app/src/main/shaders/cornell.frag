/*
 * Cornell Lights Fragment shader 
 */
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 1) uniform sampler2D tex;

layout (location = 0) in vec4 color;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 pos;
layout (location = 3) in vec3 viewDir;
layout (location = 4) in vec3 normal;

layout(std140, set = 0, binding = 2) uniform lightDefns {
	vec4 posSize[4];
	vec4 color[4];
	int numLights;
} lights;

#define saturate(x) clamp( x, 0.0, 1.0 )

layout (location = 0) out vec4 FragColor;

void main() {
  vec4 vTexColor = texture( tex, uv );

  vec3 vNormal = normalize(normal);
  
  vec3  diffuse = vec3(0.2f);
  float iter = 0.0f;
  for (int i=0; i < lights.numLights; i++)
  {
    vec3 vLightPos = lights.posSize[i].xyz - pos.xyz;
    vec3 vLightPosDir = normalize (vLightPos);
    float intensity = max(dot(vNormal,vLightPosDir), 0.1);
    if (intensity > 0.0f)
    {
      iter += 0.6f;
      diffuse += lights.color[i].xyz * (intensity + saturate( 1.0 - 0.45 * dot( vLightPos, vLightPos ) )*lights.color[i].w);
    }
  } 
  
  diffuse = min(1.0f - (pos.z / 10.0f), 1.0f) * (diffuse / iter);
  
  FragColor = vec4(color.xyz*vTexColor.xyz*diffuse , 1.0f);
}
