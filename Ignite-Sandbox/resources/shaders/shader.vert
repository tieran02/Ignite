#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Scene {
    mat4 view;
    mat4 proj;
    vec3 viewPosition;
} scene;

layout(push_constant) uniform Model
{
    mat4 model;
} model;


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangent;  

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 3) out vec3 ViewPos;
layout(location = 4) out mat3 TBN;
layout(location = 7) out vec3 Normal;

void main() 
{
    FragPos = mat4x3(model.model) * vec4(inPosition, 1.0);
    TexCoords = inTexCoord;
    Normal = inNormal;

    mat3 normalMatrix = transpose(inverse(mat3(model.model)));
    vec3 T = mat3(transpose(inverse(model.model))) * inTangent;  
    vec3 N = mat3(transpose(inverse(model.model))) * inNormal;  
    vec3 B = normalize(cross(N, T));
    TBN = mat3(T, B, N);

    ViewPos = scene.viewPosition;

    gl_Position = scene.proj * scene.view * vec4(FragPos, 1.0);
}