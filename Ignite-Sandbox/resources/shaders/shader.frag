#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() 
{
    vec3 ambient = vec3(0.25,.25,.25);
    vec3 lightDirection = vec3(-0.2, -1.0, -0.3);

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-lightDirection);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * texture(texSampler, fragTexCoord).rgb;  

    vec3 result = ambient + diffuse;
    outColor = vec4(result,1.0);
    //outColor = texture(texSampler, fragTexCoord);
}