#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
out vec2 v_texCoord;

uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * position;
    v_texCoord = texCoord;
};

#shader fragment
#version 330 core

uniform sampler1D u_Texture;
uniform vec2 u_StartPos;
uniform float u_AspectRatio;
uniform float u_Scale = 3.0;
in vec2 v_texCoord;

void main()
{
    float cen = u_Scale / 2.0;
    vec2 center = vec2(cen * u_AspectRatio, cen);
    float scale = u_Scale;
    int iter = 100;

/*
    This variant also generates interesting results
    vec2 c = vec2(0.0, 0.0);
    c.x = v_texCoord.x * scale * u_AspectRatio - center.x;
    c.y = v_texCoord.y * scale - center.y;

    int i = 0;
    vec2 z = u_StartPos;
*/
    vec2 z = vec2(0.0, 0.0);
    z.x = v_texCoord.x * scale * u_AspectRatio - center.x;
    z.y = v_texCoord.y * scale - center.y;

    int i = 0;
    vec2 c = u_StartPos;
 
    for ( i = 0; i < iter; ++i )
    {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;

        if((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }

    vec4 r = vec4(0.5, 0.0, 0.0, 1.0);
    vec4 g = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 b = vec4(0.0, 0.0, 0.0, 1.0);

    float normi = float(i)/100.0;
    normi = 2.0 * normi - 1.0;
    normi = normi * normi;
    float mixfac = exp(-4.0*normi*normi);

    gl_FragColor = g * mixfac + (1.0 - mixfac) * b;
}
