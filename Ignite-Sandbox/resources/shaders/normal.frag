#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_LIGHTS 10
struct Light {
   vec4 position;
   vec3 intensities;
   vec3 coneDirection;
   float attenuation;
   float ambientCoefficient;
   float coneAngle;
};
layout(set = 2, binding = 0) buffer Lights
{
    uint LightCount;
    Light lights[MAX_LIGHTS];
} lights;

layout(set = 1, binding = 0) uniform sampler2D DiffuseSampler;
layout(set = 1, binding = 1) uniform sampler2D SpecularSampler;
layout(set = 1, binding = 2) uniform sampler2D NormalSampler;
layout(set = 1, binding = 3) uniform sampler2D AlphaSampler;

layout(push_constant) uniform Material 
{
	layout(offset = 64) vec4 ambient;
	vec4 diffuse;
    vec4 specular;
    float shininess;
	float opacity;
} material;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 ViewPos;
layout(location = 3) in vec3 Normal;
layout(location = 4) in vec4 Tangent;

layout(location = 0) out vec4 outColor;

vec3 FetchNormalVector(vec2 texCoord)
{
    vec3 m = texture(NormalSampler, TexCoords).xyz;
    vec3 normal = vec3((2.0 * m) - 1.0);
    return normal.xyz;
}

vec3 two_component_normal(vec2 N)
{
	return vec3(N, sqrt(max(1.0 - dot(N, N), 0.0)));
}


vec3 FetchObjectNormalVector(vec2 texcoord, vec3 normal, vec4 tangent, float sigma)
{
    vec3 N = normalize(normal);
	//N.y = -N.y;
	vec3 T = normalize(tangent.xyz);
	vec3 B = cross(N, T) * tangent.w;
	mat3 TBN = mat3(T, B, N);

	vec3 tnorm = normalize(TBN * FetchNormalVector(texcoord));
    if (gl_FrontFacing)
        return tnorm;
    else
        return -tnorm;
}

void main() 
{
    if(texture(AlphaSampler, TexCoords).r == 0)
        discard;

    // get diffuse color
    vec3 color = texture(DiffuseSampler, TexCoords).rgb;
     // normal in object space
    vec3 tnorm = FetchObjectNormalVector(TexCoords,Normal,Tangent,1.0);
    //vec3 tnorm = FetchNormalVector(TexCoords);
    //vec3 tnorm = Normal;
    //vec3 normal = mat3(WorldMatrix) * tnorm;
    
    outColor = vec4((tnorm + 1.0) / 2.0, 1.0);
}