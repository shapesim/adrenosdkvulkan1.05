/*
 * Wibble Fragment shader ImageEffects demo
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
  float factor1 = sin(unidata.time + uv.y*3.14f)*0.1f;
  float factor2 = cos(unidata.time + uv.x*3.14f)*0.1f;
   
  vec2 vOffsetTexCoords = vec2(factor1, factor2);
  vec2 uv_wibbled = uv ;

  uv_wibbled += vOffsetTexCoords;
  
  uFragColor = texture( tex, uv_wibbled );
}
