#version 450
#extension GL_ARB_separate_shader_objects : enable

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
layout(location = 4) in vec3 LightPos;
layout(location = 5) in mat3 TBN;


layout(location = 0) out vec4 outColor;

void main() 
{
    if(texture(AlphaSampler, TexCoords).r == 0)
        discard;

     // obtain normal from normal map in range [0,1]
    vec3 normal = normalize(2*(texture(NormalSampler, TexCoords).rgb)-1);

    vec3 lightDirectionTangent = TBN * normalize(vec3(0,1,0));
    vec3 viewDirectionTangent = TBN * normalize(ViewPos - FragPos);

    // get diffuse color
    vec3 color = texture(DiffuseSampler, TexCoords).rgb;
    // ambient
    vec3 ambient = (0.2 * texture(DiffuseSampler, TexCoords)).rgb;

    // diffuse 
    //vec3 lightDir = normalize(vec3(-.5,1,.1));
    float diff = max(dot(lightDirectionTangent, normal), 0.0);
    vec3 diffuse = vec3(0.4,0.2,0.3)* diff * color * 2;  

    // specular
    //vec3 viewDir = normalize(TBN * normalize(ViewPos - FragPos));
    vec3 reflectDir = reflect(-viewDirectionTangent, normal); 
    vec3 halfwayDir = normalize(lightDirectionTangent + viewDirectionTangent);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 specular = material.specular.rgb * spec * texture(SpecularSampler, TexCoords).rgb;

    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, 1.0);
}