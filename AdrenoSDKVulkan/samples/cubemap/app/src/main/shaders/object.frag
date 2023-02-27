// Object Fragment Shader

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 1) uniform samplerCube cubeTexture;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inView;
layout (location = 3) in vec3 inLight;
layout (location = 4) in mat4 invModelView;

layout (location = 0) out vec4 uFragColor;
void main()
{
    vec3 incident = inPosition;
    vec3 reflected = reflect(incident, inNormal);

    // Convert from camera space to world space
    reflected = vec3(invModelView * vec4(reflected, 0.0))*vec3(-1,1,-1);

    // Get the reflected color from the cubemap and then blend with white
    vec4 color = texture(cubeTexture, reflected, 0.0);
    color = color*0.8 + vec4(1.0,1.0,1.0,1.0)*0.2;

    vec3 ambient = color.rgb * vec3(0.5);
    vec3 diffuse = color.rgb * max(dot(inNormal,  inLight), 0.0);
    vec3 reflection = reflect(-inLight, inNormal);
    vec3 specular = pow(max(dot(reflection, inView), 0.0), 16.0) * vec3(0.5,0.5,0.5);
    uFragColor = vec4(ambient + diffuse + specular, 1.0);
}
