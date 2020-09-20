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
layout (location = 2) in vec4 inTangent;
layout (location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out vec3 ViewPos;
layout(location = 3) out vec3 Normal;
layout(location = 4) out vec4 Tangent;
//layout(location = 5) out mat3 TBN;

void main() 
{
    vec3 world_col0 = model.model[0].xyz;
    vec3 world_col1 = model.model[1].xyz;
    vec3 world_col2 = model.model[2].xyz;
    vec3 world_col3 = model.model[3].xyz;

    FragPos = vec3(model.model * vec4(inPosition, 1.0));
    TexCoords = inTexCoord;

    mat3 NormalTransform = transpose(inverse(mat3(model.model)));
    Normal = normalize(NormalTransform * inNormal);
    vec3 T = vec3(normalize(NormalTransform * inTangent.xyz));
    Tangent = vec4(T, inTangent.w);
//    vec3 B =  normalize(cross(Normal, Tangent) * inTangent.w);
//
//    TBN = mat3(Tangent,B,Normal);


    ViewPos = scene.viewPosition;

    gl_Position = scene.proj * scene.view * vec4(FragPos, 1.0);
}