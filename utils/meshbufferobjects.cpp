#include <algorithm>
#include <numeric>
#include "meshbufferobjects.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<float> mbos::vbo(const mesh& m, bool flatShading)
{
#ifdef USE_ASSIMP

    const aiMesh *mesh = m._assimpMesh;
    size_t numVertices = mesh->mNumVertices;
    if (numVertices == 0)
        return {};

    auto vertexPtr = mesh->mVertices;
    auto normalPtr = mesh->mNormals;
    const aiVector3D *texcoordsPtr = nullptr;
    if (mesh->HasTextureCoords(0))
    {
        texcoordsPtr = mesh->mTextureCoords[0];
    }

    std::vector<float> vertices;
    vertices.reserve(numVertices * (3 + 3 + 2)); // pos + nor + texture coord
    for ( size_t ii = 0; ii < numVertices; ++ii)
    {
        // vertex
        vertices.push_back(vertexPtr[ii][0]);
        vertices.push_back(vertexPtr[ii][1]);
        vertices.push_back(vertexPtr[ii][2]);

        // normal
        if (mesh->HasNormals())
        {
            vertices.push_back(normalPtr[ii][0]);
            vertices.push_back(normalPtr[ii][1]);
            vertices.push_back(normalPtr[ii][2]);
        }
        else
        {
            vertices.push_back(0.5773502691896258);
            vertices.push_back(0.5773502691896258);
            vertices.push_back(0.5773502691896258);
        }

        if (texcoordsPtr)
        {
            vertices.push_back(texcoordsPtr[ii][0]);
            vertices.push_back(texcoordsPtr[ii][1]);
        }
        else
        {
            // fake texture coordinates
            vertices.push_back(0);
            vertices.push_back(0);
        }
    }

    return vertices;

#else
    std::vector<float> positions = mbos::ComputeVertices(m, flatShading);
    std::vector<float> vnormals  = mbos::ComputeVertexNormals(m, flatShading);
    std::vector<float> vtexcoord = mbos::ComputeVertexTexCoords(m, flatShading);

    size_t numVertices = positions.size() / 3;
    std::vector<float> vertices;
    vertices.reserve(numVertices * (3 + 3 + 2)); // pos + nor + texture coord

    for ( size_t ii = 0u; ii < numVertices; ++ii )
    {
        // push positions
        vertices.push_back(positions[3*ii+0]);
        vertices.push_back(positions[3*ii+1]);
        vertices.push_back(positions[3*ii+2]);
        // push normals
        vertices.push_back(vnormals[3*ii+0]);
        vertices.push_back(vnormals[3*ii+1]);
        vertices.push_back(vnormals[3*ii+2]);
        // push texture coordinates
        vertices.push_back(vtexcoord[2*ii+0]);
        vertices.push_back(vtexcoord[2*ii+1]);
    }

    return vertices;
#endif
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<unsigned int> mbos::ibo(const mesh& m, bool flatShading)
{
#ifdef USE_ASSIMP
    const aiMesh *mesh = m._assimpMesh;
    size_t numFaces = mesh->mNumFaces;
    std::vector<unsigned int> conn(3 * numFaces);
    for (size_t ii = 0; ii < numFaces; ++ii)
    {
        const aiFace &face = mesh->mFaces[ii];
        for (size_t jj = 0; jj < face.mNumIndices; ++jj)
            conn.push_back(face.mIndices[jj]);
    }

    return conn;

#else
    const std::vector<mesh::triface> &trias = m._trias;
    if ( !flatShading )
    {
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

    std::vector<unsigned int> conn(3 * m._trias.size());
    std::iota(std::begin(conn), std::end(conn), 0);
    return conn;
#endif
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<float> mbos::ComputeVertices(const mesh& m, bool flatShading)
{
    if ( !flatShading )
        return m._vertices;

    std::vector<float> vertices;
    vertices.reserve(3 * m._trias.size());
    for ( const auto& tria : m._trias )
    {
        const float *v0 = &m._vertices[3*tria[0]];
        const float *v1 = &m._vertices[3*tria[3]];
        const float *v2 = &m._vertices[3*tria[6]];
        vertices.push_back(v0[0]);
        vertices.push_back(v0[1]);
        vertices.push_back(v0[2]);
        vertices.push_back(v1[0]);
        vertices.push_back(v1[1]);
        vertices.push_back(v1[2]);
        vertices.push_back(v2[0]);
        vertices.push_back(v2[1]);
        vertices.push_back(v2[2]);
    }

    return vertices;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static const float* getNormal( const float *v0,
                               const float *v1,
                               const float *v2 )
{
    static float normal[3] = {0.0f};
    float e0[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
    float e1[3] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
    normal[0] = e0[1] * e1[2] - e1[1] * e0[2];
    normal[1] = e1[0] * e0[2] - e0[0] * e1[2];
    normal[2] = e0[0] * e1[1] - e1[0] * e0[1];
    return normal;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<float> mbos::ComputeVertexNormals(const mesh& m, bool flatShading)
{
    std::vector<float> vnormals;
    if ( m._vertices.empty() || m._trias.empty() )
        return vnormals;

    if ( flatShading )
    {
        // 3 vertex per tria
        vnormals.resize(m._trias.size() * 3 * 3, 0.0f);
    }
    else
    {
        vnormals.resize(m._vertices.size(), 0.0f);
    }

    auto smoothNormEvaluator = [&vnormals, &m](size_t tria)
    {
        const mesh::triface &tri = m._trias[tria];
        const float *n = getNormal(&m._vertices[3*tri[0]],
                                   &m._vertices[3*tri[3]],
                                   &m._vertices[3*tri[6]]);
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

    if ( !flatShading )
    {
        for ( size_t ii = 0u; ii < m._trias.size(); ++ii )
            smoothNormEvaluator(ii);
    }
    else
    {
        for ( size_t ii = 0u; ii < m._trias.size(); ++ii )
        {
            const mesh::triface &tri = m._trias[ii];
            const float *normal = getNormal(&m._vertices[3*tri[0]],
                                            &m._vertices[3*tri[3]],
                                            &m._vertices[3*tri[6]]);
            vnormals[9*ii + 0] = normal[0];
            vnormals[9*ii + 1] = normal[1];
            vnormals[9*ii + 2] = normal[2];
            vnormals[9*ii + 3] = normal[0];
            vnormals[9*ii + 4] = normal[1];
            vnormals[9*ii + 5] = normal[2];
            vnormals[9*ii + 6] = normal[0];
            vnormals[9*ii + 7] = normal[1];
            vnormals[9*ii + 8] = normal[2];
        }
    }

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
std::vector<float> mbos::ComputeVertexTexCoords(const mesh& m, bool flatShading)
{
    size_t numVertices = !flatShading ? m._vertices.size() / 3
                                      : m._trias.size() * 3;
    if ( m._textureCoords.empty() )
    {
        // no textures
        return std::vector<float>(2 * numVertices, 0.0f);
    }

    if ( flatShading )
    {
        std::vector<float> texcoord;
        texcoord.reserve(2 * numVertices);
        for ( const auto& t : m._trias )
        {
            const float *tc = &m._textureCoords[2*t[1]];
            texcoord.push_back(tc[0]);
            texcoord.push_back(tc[1]);

            tc = &m._textureCoords[2*t[4]];
            texcoord.push_back(tc[0]);
            texcoord.push_back(tc[1]);

            tc = &m._textureCoords[2*t[7]];
            texcoord.push_back(tc[0]);
            texcoord.push_back(tc[1]);
        }
        return texcoord;
    }

    std::vector<float> texcoord(2 * numVertices, -20);

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
