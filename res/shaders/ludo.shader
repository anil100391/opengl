// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#shader vertex
#version 330 core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uniform mat4 u_MVP;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void main()
{
    gl_Position = u_MVP * position;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#shader fragment
#version 330 core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
layout(location = 0) out vec4 color;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uniform int u_CellType;
uniform int u_CellIndex;
uniform int u_Highlighted = 0;
uniform float u_Size;
uniform vec2 u_CellOrigin;
uniform vec4 u_InvalidCellColor;
uniform vec4 u_RegularCellColor;
uniform vec4 u_StopCellColor;
uniform vec4 u_HighlightColor;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float plot( vec2 xy )
{
    return smoothstep( 0.0, 0.10, min( fract( xy.x ), fract( xy.y ) ) );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void main()
{
    if ( u_CellType == 0 ) // invalid sq
    {
        color = vec4( u_InvalidCellColor );
    }
    else if ( u_CellType == 1 ) // regular cell
    {
        color = u_RegularCellColor;
    }
    else if ( u_CellType == 2 ) // stop locations
    {
        color = u_StopCellColor;
    }

    if ( u_Highlighted != 0 )
        color = color * u_HighlightColor;

    vec2 coord = vec2( gl_FragCoord ) - u_CellOrigin;
    coord = coord / u_Size;
    float fac = plot( coord );
    color = color * ((1.0f - fac) * u_InvalidCellColor + fac * u_RegularCellColor);

    // vec2 coord = vec2( gl_FragCoord ) - u_CellOrigin;
    // coord = coord / u_Size;
    // color = (u_CellIndex / 225.0f) * color;
}

