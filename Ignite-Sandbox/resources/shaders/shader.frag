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

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 LightDir;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-LightDir);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * texture(texSampler, fragTexCoord).rgb * material.diffuse.rgb;

    vec4 result = vec4((material.ambient.rgb + diffuse), 1.0-material.opacity);
    outColor = result;
}