/*
 * Deferred Final Fragment shader 
 */
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout (input_attachment_index=0, set = 0, binding = 0) uniform subpassInput positionsTarget;
layout (input_attachment_index=1, set = 0, binding = 1) uniform subpassInput normalTarget;
layout (input_attachment_index=2, set = 0, binding = 2) uniform subpassInput colorTarget;

layout (location = 0) in vec4 colorVert;
layout (location = 1) in vec2 uv;

layout(std140, set = 0, binding = 3) uniform lightDefns {
	vec4 posSize[4];
	vec4 color[4];
} lights;

#define saturate(x) clamp( x, 0.0, 1.0 )

layout (location = 0) out vec4 FragColor;

void main() {

  vec4 pos      = subpassLoad( positionsTarget );
  vec4 normal   = subpassLoad( normalTarget );
  vec4 color    = subpassLoad( colorTarget );

  vec3  diffuse = vec3(0.3f);
  float iter = 0.0f;
  for (int i=0; i < 4; i++)
  {

    vec3 vLightPos = lights.posSize[i].xyz - pos.xyz;
    vec3 vLightPosDir = normalize (vLightPos);
    float intensity = max(dot(normal.xyz,vLightPosDir), 0.0);
    if (intensity > 0.0f)
    {
      diffuse += lights.color[i].xyz * intensity *lights.color[i].w;
    }
  } 
  
  diffuse = saturate(diffuse);


  FragColor = vec4(color.xyz*diffuse , 1.0f);

/* Debug render

  if (uv.y > 0.66)
  {
    FragColor = vec4(color.xyz, 1.0f);
  }
  else if (uv.y > 0.33)
  {
    FragColor = vec4(pos.xyz, 1.0f);
  }
  else
  {
    FragColor = vec4(normal.xyz, 1.0f);
  }
*/
}
