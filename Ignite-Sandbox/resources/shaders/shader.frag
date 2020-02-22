#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(push_constant) uniform Material 
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float opacity;
} material;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;
layout(location = 3) in vec3 LightDir;

layout(location = 0) out vec4 outColor;

void main() 
{
    // ambient
    vec3 ambient = vec3(0.4,0.4,0.4) * texture(texSampler, TexCoords).rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(vec3(12,12,12) - FragPos);
    vec3 lightDir = normalize(-LightDir);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(2.0,2.0,2.0) * diff * texture(texSampler, TexCoords).rgb;  

    vec3 result = ambient + diffuse;
    outColor = vec4(result, 1.0);
}