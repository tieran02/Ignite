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
layout(location = 3) in vec3 ViewPos;
layout(location = 4) in mat3 TBN;


layout(location = 0) out vec4 outColor;


vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDirectionTangent = TBN * normalize(light.position.xyz);

    // diffuse shading
    float diff = max(dot(normal, lightDirectionTangent), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-viewDir, normal); 
    vec3 halfwayDir = normalize(lightDirectionTangent + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 specular = light.intensities.length() * material.specular.rgb * spec * texture(SpecularSampler, TexCoords).rgb;

    // combine results
    vec3 diffuse = light.intensities * diff * vec3(texture(DiffuseSampler, TexCoords).rgb);
    return diffuse + specular;
}

vec3 CalculateLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //check if light is directional
    if(light.position.w == 0)
    {
        return CalculateDirectionalLight(light,normal,fragPos,viewDir);
    }

}

void main() 
{
    if(texture(AlphaSampler, TexCoords).r == 0)
        discard;

     // obtain normal from normal map in range [0,1]
    vec3 normal = normalize(2*(texture(NormalSampler, TexCoords).rgb)-1);

    vec3 viewDirectionTangent = TBN * normalize(ViewPos - FragPos);

    // get diffuse color
    vec3 color = texture(DiffuseSampler, TexCoords).rgb;
    // ambient
    vec3 ambient = (0.2 * texture(DiffuseSampler, TexCoords)).rgb;


    vec3 result = ambient;

    for(int i = 0; i < lights.LightCount; i++)
    {
        result += CalculateLight(lights.lights[i] ,normal, FragPos, viewDirectionTangent);
    }

    outColor = vec4(result, 1.0);
}