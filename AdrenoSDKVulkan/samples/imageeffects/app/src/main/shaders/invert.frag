/*
 * Invert Colors Fragment shader ImageEffects demo
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
  vec4 col = texture(tex, uv);
  uFragColor = vec4( 1.0f - col.r, 1.0f - col.g, 1.0f - col.b, col.a);
}
