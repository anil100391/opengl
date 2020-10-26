// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
out vec3 viewposition;
out vec3 viewnormal;
out vec2 v_TexCoord;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void main()
{
    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);
    viewposition = vec3(u_V * u_M * vec4(position, 1.0));
    mat3 norm_matrix = transpose(inverse(mat3(u_V * u_M)));
    viewnormal = normalize(norm_matrix * normal);
    v_TexCoord  = texCoord;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#shader fragment
#version 330 core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
in vec3 viewnormal;
in vec3 viewposition;
in vec2 v_TexCoord;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uniform Material material;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct PointLight
{
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uniform PointLight light;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uniform int u_PieceColor;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vec4 ComputePointLightContribution(PointLight plight, vec3 vposition, vec3 vnormal)
{
    vec4 texColor = vec4(vec3(0.7), 1.0);

    // ambient
    // vec4 ambient = plight.ambient * material.ambient * texColor;

    // diffuse
    vec3 lightDir = normalize(plight.position - vposition);
    float diffuseStrength = max(dot(lightDir, vnormal), 0.0);
    //vec4 diffuse = diffuseStrength * plight.diffuse * material.diffuse;
    vec4 diffuse = diffuseStrength * plight.diffuse * texColor;

    // specular
    vec3 camerapos = vec3(0.0);
    vec3 viewDir = normalize(camerapos - vposition);
    vec3 reflectDir = reflect(-lightDir, vnormal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec4 specular = specularStrength * plight.specular * material.specular;
    vec4 specular = specularStrength * plight.specular * texColor;

    // gl_FragColor = ambient + diffuse + specular;

    float d = length(plight.position - vposition);
    float attenuation = 10.0 / (1.0 + 0.09 * d + 0.032 * d * d);
    return (diffuse + specular) * attenuation;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct DirectionalLight
{
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vec4 ComputeDirectionalLightContribution(DirectionalLight dlight, vec3 vposition, vec3 vnormal)
{
    vec4 texColor = u_PieceColor == 0 ? vec4(vec3(0.2), 1.0) : vec4(vec3(0.8), 1.0);

    // ambient
    vec4 ambient = dlight.ambient * texColor;

    // diffuse
    vec3 lightDir = normalize(dlight.direction);
    float diffuseStrength = max(dot(-lightDir, vnormal), 0.0);
    //vec4 diffuse = diffuseStrength * dlight.diffuse * material.diffuse;
    vec4 diffuse = diffuseStrength * dlight.diffuse * texColor;

    // specular
    vec3 camerapos = vec3(0.0);
    vec3 viewDir = normalize(camerapos - vposition);
    vec3 reflectDir = reflect(lightDir, vnormal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    //vec4 specular = specularStrength * dlight.specular * material.specular;
    vec4 specular = specularStrength * dlight.specular * texColor;

    // gl_FragColor = ambient + diffuse + specular;

    return (ambient + diffuse + specular);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float cutoff;
    float cutofffinal;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vec4 ComputeSpotLightContribution(SpotLight slight, vec3 vposition, vec3 vnormal)
{
    vec4 texColor = vec4(vec3(0.7), 1.0);

    // ambient
    // vec4 ambient = slight.ambient * material.ambient * texColor;

    // diffuse
    vec3 lightDir = normalize(slight.position - vposition);
    vec3 slightDir = normalize(slight.direction);

    float contrib = (dot(lightDir, slightDir) - slight.cutofffinal)/(slight.cutoff - slight.cutofffinal);
    contrib == clamp(contrib, 0.0, 1.0);

    float diffuseStrength = max(dot(lightDir, vnormal), 0.0);
    diffuseStrength *= contrib;
    //vec4 diffuse = diffuseStrength * slight.diffuse * material.diffuse;
    vec4 diffuse = diffuseStrength * slight.diffuse * texColor;

    // specular
    vec3 camerapos = vec3(0.0);
    vec3 viewDir = normalize(camerapos - vposition);
    vec3 reflectDir = reflect(-lightDir, vnormal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    specularStrength *= contrib;
    //vec4 specular = specularStrength * slight.specular * material.specular;
    vec4 specular = specularStrength * slight.specular * texColor;

    // gl_FragColor = ambient + diffuse + specular;

    float d = length(slight.position - vposition);
    float attenuation = 10.0 / (1.0 + 0.09 * d + 0.032 * d * d);
    return (diffuse + specular) * attenuation;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uniform sampler2D u_Texture;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void main()
{
    // gl_FragColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
    DirectionalLight dlight;
    dlight.direction = vec3( 0.0, 0.0, -1.0 );
    dlight.ambient = vec4( vec3( 0.4 ), 1.0 );
    dlight.diffuse = vec4( vec3( 0.8 ), 1.0 );
    dlight.specular  = vec4( vec3( 0.7 ), 1.0 );

    /*
    SpotLight slight;
    slight.position  = light.position;
    slight.direction = vec3(0.0, 0.0, 1.0);
    slight.ambient   = light.ambient;
    slight.diffuse   = light.diffuse;
    slight.specular  = light.specular;
    slight.cutoff    = 0.85;
    slight.cutofffinal = 0.84;
    */

    gl_FragColor = ComputeDirectionalLightContribution( dlight, viewposition, viewnormal );
                   // ComputeSpotLightContribution(slight, viewposition, viewnormal);
}
