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

void main()
{
    float strength = max(dot(normalize(viewnormal), normalize(-viewposition)), 0.0);
    gl_FragColor = strength * vec4(1.0);
};

