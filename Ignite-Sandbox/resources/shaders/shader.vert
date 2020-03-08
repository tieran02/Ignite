#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPosition;
    vec3 LightPos;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;  

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 3) out vec3 ViewPos;
layout(location = 4) out vec3 LightPos;
layout(location = 5) out mat3 TBN;
layout(location = 8) out vec3 DebugNormal;

void main() 
{
    FragPos = vec3(ubo.model * vec4(inPosition, 1.0));
    TexCoords = inTexCoord;

    vec3 T = normalize(vec3(ubo.model * vec4(inTangent,   0.0)));
    vec3 B = normalize(vec3(ubo.model * vec4(inBitangent, 0.0)));
    vec3 N = normalize(vec3(ubo.model * vec4(inNormal,    0.0)));
    TBN = transpose(mat3(T, B, N));

    ViewPos = ubo.viewPosition;
    LightPos = ubo.LightPos;
    DebugNormal = mat3(transpose(inverse(ubo.model))) * inNormal;

    gl_Position = ubo.proj * ubo.view * vec4(FragPos, 1.0);
}