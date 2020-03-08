#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D DiffuseSampler;
layout(set = 1, binding = 1) uniform sampler2D SpecularSampler;
layout(set = 1, binding = 2) uniform sampler2D NormalSampler;
layout(set = 1, binding = 3) uniform sampler2D AlphaSampler;

layout(push_constant) uniform Material 
{
	vec4 ambient;
	vec4 diffuse;
    vec4 specular;
    float shininess;
	float opacity;
} material;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;
layout(location = 3) in vec3 ViewPos;
layout(location = 4) in vec3 LightDir;

layout(location = 0) out vec4 outColor;

void main() 
{
    if(texture(AlphaSampler, TexCoords).r == 0)
        discard;

    // ambient
    vec3 ambient = vec3(0.4,0.4,0.4) * texture(DiffuseSampler, TexCoords).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(vec3(12,12,12) - FragPos);
    vec3 lightDir = normalize(-LightDir);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(2.0,2.0,2.0) * diff * texture(DiffuseSampler, TexCoords).rgb;  

    // specular
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular.rgb * spec * texture(SpecularSampler, TexCoords).rgb;  
    //vec3 specular = spec * texture(SpecularSampler, TexCoords).rgb;  

    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, material.opacity);
}