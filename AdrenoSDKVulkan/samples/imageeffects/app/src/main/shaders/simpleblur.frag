/*
 * Simple Blur Fragment shader ImageEffects demo
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


#define WIDTH 256.0f
#define HEIGHT 256.0f

layout (location = 0) out vec4 uFragColor;
void main() {
  float factor = abs(sin(unidata.time));
  vec4 blur = factor * texture(tex, uv + vec2(0.0f,  2.0f/WIDTH)) +
              factor * texture(tex, uv + vec2(0.0f, -2.0f/WIDTH)) +
              factor * texture(tex, uv + vec2( 2.0f/WIDTH, 0.0f)) +
              factor * texture(tex, uv + vec2(-2.0f/WIDTH, 0.0f)) +
              texture(tex, uv);
              
   uFragColor = blur / (1.0f + 4.0f*factor);
}
