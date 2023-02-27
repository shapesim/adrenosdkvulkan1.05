/*
 * Basic Deferred target shader
 */
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 1) uniform sampler2D tex;
layout (set = 0, binding = 2) uniform sampler2D normaltex;

layout (location = 0) in vec4 color;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 pos;
layout (location = 3) in vec4 viewDir;
layout (location = 4) in vec4 normal;

layout (location = 0) out vec4 PositionOut;
layout (location = 1) out vec4 NormalOut;
layout (location = 2) out vec4 ColorOut;

#define saturate(x) clamp( x, 0.0, 1.0 )

void main() {
  PositionOut = pos;
  PositionOut.a = 1.0f;
  if ((uv.x == 0) && (uv.y == 0) )
  {
    // Test models have non-uv mapped areas, this is an attempt to account for that.
    NormalOut = normal;
  } else {
    NormalOut = normal * (texture( normaltex, uv ) * 2.0f - 1.0f);
  }
  ColorOut = color*texture( tex, uv );
  ColorOut.a = 1.0f;
}
