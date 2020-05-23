#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

out vec3 viewposition;
out vec3 viewnormal;
out vec2 v_TexCoord;

void main()
{
    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);
    viewposition = vec3(u_V * u_M * vec4(position, 1.0));
    mat3 norm_matrix = transpose(inverse(mat3(u_V * u_M)));
    viewnormal = normalize(norm_matrix * normal);
    v_TexCoord  = texCoord;
};

#shader fragment
#version 330 core

in vec3 viewnormal;
in vec3 viewposition;
in vec2 v_TexCoord;

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform Material material;

struct PointLight
{
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

uniform PointLight light;

uniform sampler2D u_Texture;

void main()
{
    // vec4 texColor = texture(u_Texture, v_TexCoord);
    vec4 texColor = vec4(vec3(0.7), 1.0);

    // ambient
    vec4 ambient = light.ambient * material.ambient * texColor;

    // diffuse
    vec3 lightDir = normalize(light.position - viewposition);
    float diffuseStrength = max(dot(lightDir, viewnormal), 0.0);
    //vec4 diffuse = diffuseStrength * light.diffuse * material.diffuse;
    vec4 diffuse = diffuseStrength * light.diffuse * texColor;

    // specular
    vec3 camerapos = vec3(0.0);
    vec3 viewDir = normalize(camerapos - viewposition);
    vec3 reflectDir = reflect(-lightDir, viewnormal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec4 specular = specularStrength * light.specular * material.specular;
    vec4 specular = specularStrength * light.specular * texColor;

    gl_FragColor = ambient + diffuse + specular;
};

