// Fragment shader

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 pos;
layout (location = 1) in vec3 normal;

layout (location = 0) out vec4 uFragColor;

void main()
{
  vec4 Llightpos  = vec4(-100.0f,  25.0f, -3.0f, 0.0f);
  vec4 Llightpos2 = vec4(  15.0f,  10.0f,  3.0f, 0.0f);

  vec4 Ldiffuse   = vec4(  0.5f,   0.5f,  0.4f, 1.0f);


  vec3 vNormal = normalize(-normal);
  vec3 diffuse =  Ldiffuse.xyz;

  vec3 vLightPos =  Llightpos.xyz - pos.xyz;
  vec3 vLightPosDir = normalize (vLightPos);

  vec3 vLightPos2 =  Llightpos2.xyz - pos.xyz;
  vec3 vLightPosDir2 = normalize (vLightPos2);

  float intensity = max(dot(vNormal,vLightPosDir), 0.3f);
  intensity += max(dot(vNormal,vLightPosDir2), 0.3f);

  uFragColor = vec4(intensity, 0.0f, 0.0f, 1.0f);
}
