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
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;  
layout (location = 4) in vec2 inTexCoord;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 3) out vec3 ViewPos;
layout(location = 4) out vec3 Normal;
layout(location = 5) out vec3 Tangent;

void main() 
{
    FragPos = vec3(model.model * vec4(inPosition, 1.0));
    TexCoords = inTexCoord;

    mat4 normalMatrix = transpose(inverse(model.model));
    Normal = normalize(vec3(normalMatrix * vec4(inNormal, 0.0)));
    Tangent = normalize(vec3(normalMatrix * vec4(inTangent, 0.0)));

    ViewPos = scene.viewPosition;

    gl_Position = scene.proj * scene.view * vec4(FragPos, 1.0);
}