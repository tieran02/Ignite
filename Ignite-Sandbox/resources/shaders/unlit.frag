#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 10
struct Light {
   vec4 position;
   vec3 intensities;
   vec3 coneDirection;
   float attenuation;
   float ambientCoefficient;
   float coneAngle;
};
layout(set = 2, binding = 0) buffer Lights
{
    uint LightCount;
    Light lights[MAX_LIGHTS];
} lights;

layout(set = 1, binding = 0) uniform sampler2D DiffuseSampler;
layout(set = 1, binding = 1) uniform sampler2D SpecularSampler;
layout(set = 1, binding = 2) uniform sampler2D NormalSampler;
layout(set = 1, binding = 3) uniform sampler2D AlphaSampler;

layout(push_constant) uniform Material 
{
	layout(offset = 64) vec4 ambient;
	vec4 diffuse;
    vec4 specular;
    float shininess;
	float opacity;
} material;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 3) in vec3 ViewPos;
layout(location = 4) in mat3 TBN;
layout(location = 7) in vec3 Normal;


layout(location = 0) out vec4 outColor;

void main() 
{
    if(texture(AlphaSampler, TexCoords).r == 0)
        discard;

    // get diffuse color
    vec3 color = texture(DiffuseSampler, TexCoords).rgb;

     // normal in tangent space
    vec3 normal = texture(NormalSampler, TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
    normal = normalize(TBN * normal);
    //vec3 normal = normalize(2*(texture(NormalSampler, TexCoords).rgb)-1);
    
    outColor = vec4(normal, 1.0);
}