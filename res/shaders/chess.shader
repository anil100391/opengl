#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform int u_Cell;

void main()
{
    if ( u_Cell % 3 == 0 )
        gl_FragColor = vec4(0.1, 0.1, 0.1, 1.0f);
    else
        gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0f);
};

