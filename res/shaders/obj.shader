#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

out vec3 viewposition;
out vec3 viewnormal;

void main()
{
    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);
    viewposition = vec3(u_V * u_M * vec4(position, 1.0));
    mat3 norm_matrix = transpose(inverse(mat3(u_V * u_M)));
    viewnormal = normalize(norm_matrix * normal);
};

#shader fragment
#version 330 core

in vec3 viewnormal;
in vec3 viewposition;

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

void main()
{
    // ambient
    vec4 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 lightDir = normalize(light.position - viewposition);
    float diffuseStrength = max(dot(lightDir, viewnormal), 0.0);
    vec4 diffuse = diffuseStrength * light.diffuse * material.diffuse;

    // specular
    vec3 camerapos = vec3(0.0);
    vec3 viewDir = normalize(camerapos - viewposition);
    vec3 reflectDir = reflect(-lightDir, viewnormal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec4 specular = specularStrength * light.specular * material.specular;

    gl_FragColor = ambient + diffuse + specular;
};

