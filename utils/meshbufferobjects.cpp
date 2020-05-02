#include <algorithm>
#include "meshbufferobjects.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<float> mbos::vbo(const mesh& m)
{
    const std::vector<float> &mpos = m._vertices;
    size_t numVertices             = mpos.size() / 3;

    std::vector<float> vertices;
    vertices.reserve(numVertices * (3 + 3 + 2)); // pos + nor + texture coord

    std::vector<float> vnormals  = mbos::ComputeVertexNormals(m);
    std::vector<float> vtexcoord = mbos::ComputeVertexTexCoords(m);

    for ( size_t ii = 0u; ii < numVertices; ++ii )
    {
        // push positions
        vertices.push_back(mpos[3*ii+0]);
        vertices.push_back(mpos[3*ii+1]);
        vertices.push_back(mpos[3*ii+2]);
        // push normals
        vertices.push_back(vnormals[3*ii+0]);
        vertices.push_back(vnormals[3*ii+1]);
        vertices.push_back(vnormals[3*ii+2]);
        // push texture coordinates
        vertices.push_back(vtexcoord[2*ii+0]);
        vertices.push_back(vtexcoord[2*ii+1]);
    }

    return vertices;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<unsigned int> mbos::ibo(const mesh& m)
{
    const std::vector<mesh::triface> &trias = m._trias;
    std::vector<unsigned int> conn;
    conn.reserve( 3 * trias.size() );
    std::for_each( trias.begin(), trias.end(),
                   [&conn]( const mesh::triface &f )
                   {
                       conn.push_back( f[0] );
                       conn.push_back( f[3] );
                       conn.push_back( f[6] );
                   } );
    return conn;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<float> mbos::ComputeVertexNormals(const mesh& m)
{
    std::vector<float> vnormals;
    if ( m._vertices.empty() || m._trias.empty() )
        return vnormals;

    vnormals.resize(m._vertices.size(), 0.0f);

    auto evalTriaNorm = [&vnormals, &m](unsigned int tria)
    {
        const mesh::triface &tri = m._trias[tria];
        const float *v0 = &m._vertices[3*tri[0]];
        const float *v1 = &m._vertices[3*tri[3]];
        const float *v2 = &m._vertices[3*tri[6]];
        float e0[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        float e1[3] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
        float n[3] = { e0[1] * e1[2] - e1[1] * e0[2],
                       e1[0] * e0[2] - e0[0] * e1[2],
                       e0[0] * e1[1] - e1[0] * e0[1] };

        vnormals[3*tri[0] + 0] += n[0];
        vnormals[3*tri[0] + 1] += n[1];
        vnormals[3*tri[0] + 2] += n[2];

        vnormals[3*tri[3] + 0] += n[0];
        vnormals[3*tri[3] + 1] += n[1];
        vnormals[3*tri[3] + 2] += n[2];

        vnormals[3*tri[6] + 0] += n[0];
        vnormals[3*tri[6] + 1] += n[1];
        vnormals[3*tri[6] + 2] += n[2];
    };

    for ( size_t ii = 0u; ii < m._trias.size(); ++ii )
        evalTriaNorm(ii);

    for ( int ii = 0; ii < vnormals.size() / 3 ; ++ii )
    {
        float *nor = &vnormals[3*ii];
        float len = std::sqrt(nor[0] * nor[0] + nor[1] * nor[1]  + nor[2] * nor[2]);
        if ( len > 0 )
        {
            nor[0] /= len;
            nor[1] /= len;
            nor[2] /= len;
        }
    }

    return vnormals;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<float> mbos::ComputeVertexTexCoords(const mesh& m)
{
    size_t numVertices = m._vertices.size() / 3;
    std::vector<float> texcoord(2* numVertices, -20);
    if ( m._textureCoords.empty() )
    {
        // no textures
        return std::vector<float>(2 * numVertices, 0.0f);
    }

    for ( const auto& t : m._trias )
    {
        int vid = t[0];
        int tid = t[1];
        const float *tc = &m._textureCoords[2*tid];
        if ( texcoord[2*vid + 0] < -10 )
            texcoord[2*vid + 0] = tc[0];
        if ( texcoord[2*vid + 1] < -10 )
            texcoord[2*vid + 1] = tc[1];

        vid = t[3];
        tid = t[4];
        tc = &m._textureCoords[2*tid];
        if ( texcoord[2*vid + 0] < -10 )
            texcoord[2*vid + 0] = tc[0];
        if ( texcoord[2*vid + 1] < -10 )
            texcoord[2*vid + 1] = tc[1];

        vid = t[6];
        tid = t[7];
        tc = &m._textureCoords[2*tid];
        if ( texcoord[2*vid + 0] < -10 )
            texcoord[2*vid + 0] = tc[0];
        if ( texcoord[2*vid + 1] < -10 )
            texcoord[2*vid + 1] = tc[1];
    }

    return texcoord;
}
