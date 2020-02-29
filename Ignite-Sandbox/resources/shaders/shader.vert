#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;
    vec3 lightDir;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;
layout(location = 3) out vec3 ViewPos;
layout(location = 4) out vec3 LightDir;

void main() 
{
    FragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    Normal = Normal = mat3(transpose(inverse(ubo.model))) * inNormal;
    TexCoords = inTexCoord;

    LightDir = ubo.lightDir;
    ViewPos = ubo.viewPosition;

    gl_Position = ubo.proj * ubo.view * vec4(FragPos, 1.0);
}