#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_MVP;

out vec3 vnormal;

void main()
{
    gl_Position = u_MVP * vec4(position, 1.0);
    vnormal = normal;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
in vec3 vnormal;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{
    float strength = abs(dot(vnormal, vec3(1.0, 1.0, 0.4)));
    color = strength * vec4(1.0, 1.0, 0.0, 1.0);
};

