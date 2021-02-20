// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
#shader vertex
#version 330 core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
uniform mat4 u_M;
uniform mat4 u_V;
uniform mat4 u_P;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
out vec2 vTexCoord;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void main()
{
    gl_Position = u_P * u_V * u_M * vec4(position, 0.0, 1.0);
    vTexCoord = texCoord;
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
in vec2 vTexCoord;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float plot( vec2 xy )
{
    return smoothstep( 0.0, 0.15, abs( fract( xy.x ) - fract( xy.y ) ) );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool isDarkSquare( int squareIndex )
{
    int row = squareIndex / 8;
    int col = squareIndex % 8;
    return (row % 2 == col % 2);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void main()
{
    int rank = int(floor( vTexCoord.y * 8.0 ));
    int file = int(floor( vTexCoord.x * 8.0 ));
    int sq = rank * 8 + file;

    if ( isDarkSquare( sq ) )
        color = vec4( vec3( 0.2f ), 1.0f );
    else
        color = vec4( vec3( 1.0f ), 1.0f );
}
