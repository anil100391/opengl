#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;
void main()
{
    gl_Position = u_P * u_V * u_M * vec4(position, 1.0);
};

#shader fragment
#version 330 core

uniform vec4 u_Color;

void main()
{
    gl_FragColor = u_Color;
};


