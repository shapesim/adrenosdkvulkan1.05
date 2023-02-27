/*
 * Basic Normalmapping Fragment shader 
 */
#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (set = 0, binding = 1) uniform sampler2D tex;
layout (set = 0, binding = 2) uniform sampler2D normaltex;

layout (location = 0) in vec4 color;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 lightDir;
layout (location = 3) in vec3 viewDir;
layout (location = 4) in vec3 normal;


#define saturate(x) clamp( x, 0.0, 1.0 )

layout (location = 0) out vec4 uFragColor;
void main() {
    float fAmbient = 0.5f;
    vec4 vBaseColor = texture( tex, uv );
    
    // Select the normal in the appropriate space
    vec3 vNormal    = texture( normaltex, uv, -1.0 ).xyz * 2.0 - 1.0;

    // Standard Phong lighting
    float fAtten    = saturate( 1.0 - 0.05 * dot( lightDir, lightDir ) );
    vec3  vLight    = normalize( lightDir );
    vec3  vView     = normalize( viewDir );
    vec3  vHalf     = normalize( vLight + vView );
    float fDiffuse  = saturate( dot( vLight, vNormal ) );
    float fSpecular = pow( saturate( dot( vHalf, vNormal ) ), 16.0 );

    uFragColor = ( fDiffuse * fAtten + fAmbient ) * vBaseColor + ( fSpecular * fAtten ) * 0.7;
}
