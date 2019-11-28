#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_MVP;
uniform mat4 u_MV;

out vec3 viewnormal;

void main()
{
    gl_Position = u_MVP * vec4( position, 1.0 );
    viewnormal = normalize( abs( vec3( u_MV * vec4( normal, 0.0 ) ) ) );
};

#shader fragment
#version 330 core

in vec3 viewnormal;

void main()
{
    gl_FragColor = vec4(viewnormal, 1.0);
};
