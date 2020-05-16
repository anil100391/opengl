// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
out vec2 v_texCoord;

uniform mat4 u_MVP;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void main()
{
    gl_Position = u_MVP * position;
    v_texCoord = texCoord;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#shader fragment
#version 330 core

uniform vec2 u_C0;
uniform float u_AspectRatio = 1.0; // spax / spany
uniform float u_SpanY = 3.0;
uniform vec2  u_Center;
uniform int   u_Mode = 0;
in vec2 v_texCoord;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct complex
{
    float x, y;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
complex mult(complex a, complex b)
{
    complex res;
    res.x = a.x * b.x - a.y * b.y;
    res.y = a.x * b.y + b.x * a.y;
    return res;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vec2 getFragmentCoordinate(vec2 texcoord)
{
    float spanX = u_AspectRatio * u_SpanY;
    float spanY = u_SpanY;
    vec2 minCorner = u_Center - vec2(0.5 * spanX, 0.5 * spanY);

    // texcoord between 0 to 1
    float x = minCorner.x + texcoord.x * spanX;
    float y = minCorner.y + texcoord.y * spanY;
    return vec2(x, y);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vec4 juliaShader(vec2 texcoord)
{
    int iter = 100;

    vec2 z = getFragmentCoordinate(v_texCoord);

    int i = 0;

    for ( i = 0; i < iter; ++i )
    {
        float x = (z.x * z.x - z.y * z.y) + u_C0.x;
        float y = (z.y * z.x + z.x * z.y) + u_C0.y;

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

    return g * mixfac + (1.0 - mixfac) * b;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vec4 mandelbrotShader(vec2 texcoord)
{
    int iter = 100;

    vec2 z = vec2(0.0);
    vec2 c0 = getFragmentCoordinate(v_texCoord);

    int i = 0;
 
    for ( i = 0; i < iter; ++i )
    {
        float x = (z.x * z.x - z.y * z.y) + c0.x;
        float y = (z.y * z.x + z.x * z.y) + c0.y;

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

    return g * mixfac + (1.0 - mixfac) * b;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vec4 newtonShader(vec2 texcoord)
{
    int iter = 100;

    vec2 z = getFragmentCoordinate(v_texCoord);

    int i = 0;
 
    // z -> z - f(z) / f'(z)
    // for f(z) = z^3 - 1
    // z -> z - (z^3 - 1) / (3z^2)
    // z -> (2z^3 +  1) / (3z^2)

    for ( i = 0; i < iter; ++i )
    {
        complex zn;
        zn.x = z.x;
        zn.y = z.y;

        complex zsq = mult(zn, zn);
        complex zcu = mult(zsq, zn);

        zcu.x = (zcu.x * 2.0f + 1.0f) / 3.0f;
        zcu.y = (zcu.y * 2.0f) / 3.0f;

        complex zsqconj;
        zsqconj.x = zsq.x;
        zsqconj.y = -zsq.y;

        complex numerator = mult(zcu, zsqconj);
        float denominator = zsq.x * zsq.x + zsq.y * zsq.y;

        numerator.x /= denominator;
        numerator.y /= denominator;

        z.x = numerator.x;
        z.y = numerator.y;
    }

    vec4 r = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 g = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 b = vec4(0.0, 0.0, 1.0, 1.0);

    float pi = 3.141592653589793;

    vec2 fragcoord = getFragmentCoordinate(v_texCoord);

    float dist = length(fragcoord-z);
    float wt = exp(-dist * dist);

    float d0 = length(z-vec2(1.0, 0));
    float d1 = length(z-vec2(cos(2.0f*pi/3.0f), sin(2.0f*pi/3.0f)));
    float d2 = length(z-vec2(cos(4.0f*pi/3.0f), sin(4.0f*pi/3.0f)));

    float sum = d0 + d1 + d2;
    d0 /= sum;
    d1 /= sum;
    d2 /= sum;

    return r * d0 * wt + g * d1 * wt + b * d2 * wt;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void main()
{
    if ( u_Mode == 0 )
        gl_FragColor = juliaShader(v_texCoord);
    else if ( u_Mode == 1 )
        gl_FragColor = mandelbrotShader(v_texCoord);
    else
        gl_FragColor = newtonShader(v_texCoord);
}
