/*
 * Deferred Inspect Fragment shader 
 */
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 0) uniform sampler2D positionsTex;
layout (set = 0, binding = 1) uniform sampler2D normalTex;
layout (set = 0, binding = 2) uniform sampler2D colorTex;

layout (location = 0) in vec4 colorVert;
layout (location = 1) in vec2 uv;
layout (location = 2) flat in int vID;

layout(std140, set = 0, binding = 3) uniform lightDefns {
	vec4 posSize[4];
	vec4 color[4];
} lights;

#define saturate(x) clamp( x, 0.0, 1.0 )

layout (location = 0) out vec4 FragColor;

void main() {
  if (vID <= 3)
  {
    float d = 1.0f-(texture( positionsTex, uv ).z+4)/8.0f;
    FragColor = vec4(d, d, d, 1.0f);
  }
  else if  (vID <= 9)
  {
    FragColor = vec4(texture( normalTex, uv ).xyz, 1.0f);
  }
  else
  {
    FragColor = vec4(texture( colorTex, uv ).xyz, 1.0f);
  }
}
