#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Scene {
    mat4 view;
    mat4 proj;
    vec3 viewPosition;
    vec3 LightPos;
} scene;

layout(set = 1, binding = 0) uniform Model
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

void main() 
{
    FragPos = vec3(model.model * vec4(inPosition, 1.0));
    TexCoords = inTexCoord;

    gl_Position = scene.proj * scene.view * vec4(FragPos, 1.0);
}