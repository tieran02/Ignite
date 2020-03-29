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
layout(location = 4) in vec3 Normal;
layout(location = 5) in vec3 Tangent;


layout(location = 0) out vec4 outColor;

vec3 FetchNormalVector(vec2 texCoord)
{
    vec2 m = texture(NormalSampler, TexCoords).xy;
    return vec3(m ,sqrt(1.0 - m.x * m.x - m.y * m.y));
}

vec3 FetchObjectNormalVector(vec2 texcoord, vec3 normal, vec3 tangent, float sigma)
{
    vec3 m = FetchNormalVector(texcoord);
    vec3 n = normalize(normal);
    vec3 t = normalize(tangent - n * dot(tangent,n));
    vec3 b = cross(normal, tangent) * sigma;
    return (t * m.x + b * m.y + n * m.z);
}

vec3 SpecularPhong(vec3 LightDirectionTangent, vec3 normal,vec3 ViewDirectionTangent)
{
    // specular shading
    vec3 reflectDir = reflect(-ViewDirectionTangent, normal); 
    vec3 halfwayDir = normalize(LightDirectionTangent + ViewDirectionTangent);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    return material.specular.rgb * spec * texture(SpecularSampler, TexCoords).rgb;
}

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDirectionTangent = normalize(light.position.xyz);

    // diffuse shading
    float diff = max(dot(normal, lightDirectionTangent), 0.0);
    vec3 diffusePhong = vec3(texture(DiffuseSampler, TexCoords).rgb) * diff;

    vec3 specularPhong = SpecularPhong(lightDirectionTangent, normal, viewDir);

    return (light.intensities * (diffusePhong + specularPhong));
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 LightDirection = light.position.xyz - FragPos;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    // diffuse shading
    float diff = max(0.0 ,dot(normal,LightDirection));
    //vec3 diffusePhong = SurfaceColour * diffuse_intensity;
    vec3 diffusePhong = diff * vec3(texture(DiffuseSampler, TexCoords));
    
    // combine results
    vec3 specularPhong = SpecularPhong(LightDirection, normal, viewDir);

    // attenuation
    float attenuation = smoothstep(light.attenuation, light.attenuation/2, Distance);

    return (light.intensities * (diffusePhong + specularPhong)) * attenuation;
}

vec3 CalculateLight(Light light, vec3 normal, vec3 viewDir)
{
    //check if light is directional
    if(light.position.w == 0)
    {
        return CalculateDirectionalLight(light,normal,viewDir);
    }
    return CalculatePointLight(light,normal,viewDir);
}

void main() 
{
    if(texture(AlphaSampler, TexCoords).r == 0)
        discard;

     // obtain normal from normal map in range [0,1] in world space
    vec3 normTexture = FetchObjectNormalVector(TexCoords,Normal,Tangent,1.0);

    vec3 viewDirectionTangent = normalize(ViewPos - FragPos);

    // get diffuse color
    vec3 color = texture(DiffuseSampler, TexCoords).rgb;
    // ambient
    vec3 ambient = (0.2 * texture(DiffuseSampler, TexCoords)).rgb;


    vec3 result = ambient;

    for(int i = 0; i < lights.LightCount; i++)
    {
        result += CalculateLight(lights.lights[i] ,normTexture, viewDirectionTangent);
    }

    outColor = vec4(result, 1.0);
}