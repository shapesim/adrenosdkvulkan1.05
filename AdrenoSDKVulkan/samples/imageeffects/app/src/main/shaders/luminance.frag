/*
 * Luminance Fragment shader ImageEffects demo
 */
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (set = 0, binding = 0) uniform sampler2D tex;
layout (std140, set = 0, binding = 1) uniform uni
                                      {
                                        float time;
                                      } unidata;

layout (location = 0) in vec4 color;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec4 uFragColor;
void main() {
  vec3  vSample   = texture( tex, uv ).rgb;
  float fLuminance = dot( vSample, vec3( 0.3, 0.59, 0.11 ) );
  uFragColor = vec4(fLuminance, fLuminance, fLuminance, 1.0f);
}
