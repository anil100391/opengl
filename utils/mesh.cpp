#include "mesh.h"
#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool parseline(const std::string &objline, mesh& m);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
mesh::mesh(const char* filename)
    : _name(filename)
{
    std::ifstream obj(filename);
    if ( !obj.is_open() )
    {
        std::cerr << "Failed to open file " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(obj, line))
    {
        parseline(line, *this);
    }

    std::cout << _name << " stats\n"
                       << " vertices: " << _vertices.size() / 3 << "\n"
                       << " trias: " << _trias.size() / 3 << "\n"
                       << " quads: " << _quads.size() / 4 << "\n";
    ComputeNormals();
    ComputeCog();
    ComputeBBox();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ParseVertex(std::string_view vdata, mesh &m)
{
    float x = 0, y = 0, z = 0;
    if ( 3 == sscanf(vdata.data(), "%f %f %f", &x, &y, &z) )
    {
        m._vertices.push_back(x);
        m._vertices.push_back(y);
        m._vertices.push_back(z);
    }
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ParseNormal(std::string_view vdata, mesh &m)
{
    float x = 0, y = 0, z = 0;
    if ( 3 == sscanf(vdata.data(), "%f %f %f", &x, &y, &z) )
    {
        float len = std::sqrt(x*x + y*y  +z*z);
        x /= len;
        y /= len;
        z /= len;
        m._normals.push_back(x);
        m._normals.push_back(y);
        m._normals.push_back(z);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void ParseFace(std::string_view vdata, mesh &m)
{
    int nodes[4] = {0};
    if ( 4 == sscanf(vdata.data(), "%d %d %d %d", &nodes[0], &nodes[1], &nodes[2], &nodes[3]) )
    {
        m._quads.push_back(nodes[0] - 1);
        m._quads.push_back(nodes[1] - 1);
        m._quads.push_back(nodes[2] - 1);
        m._quads.push_back(nodes[3] - 1);
    }
    else if ( 3 == sscanf(vdata.data(), "%d %d %d", &nodes[0], &nodes[1], &nodes[2]) )
    {
        m._trias.push_back(nodes[0] - 1);
        m._trias.push_back(nodes[1] - 1);
        m._trias.push_back(nodes[2] - 1);
    }
    else
    {
        assert(false);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool parseline(const std::string &line, mesh& m)
{
    if (line.size() < 2)
    {
        return false;
    }

    switch (line[0])
    {
    case 'o':
        m._name = line;
        m._name.erase(0, 2);
        std::cout << "Reading mesh data for object: " << m._name << "\n";
        break;
    case 'v':
        if (line[1] == 'n')
            ParseNormal(std::string_view(&line[3]), m);
        else if (line[1] == ' ')
            ParseVertex(std::string_view(&line[2]), m);
        break;
    case 'f':
        ParseFace(std::string_view(&line[2]), m);
        break;
    }

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::ComputeNormals()
{
    if ( _vertices.empty() || _trias.empty() )
        return;

    _normals.clear();
    _normals.resize(_vertices.size(), 0.0f);

    auto evalTriaNorm = [this](unsigned int tria)
    {
        float *v0 = &_vertices[3*_trias[3*tria]];
        float *v1 = &_vertices[3*_trias[3*tria + 1]];
        float *v2 = &_vertices[3*_trias[3*tria + 2]];
        float e0[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        float e1[3] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
        float n[3] = { e0[1] * e1[2] - e1[1] * e0[2],
                       e1[0] * e0[2] - e0[0] * e1[2],
                       e0[0] * e1[1] - e1[0] * e0[1] };

        _normals[3*_trias[3*tria] + 0] += n[0];
        _normals[3*_trias[3*tria] + 1] += n[1];
        _normals[3*_trias[3*tria] + 2] += n[2];

        _normals[3*_trias[3*tria + 1] + 0] += n[0];
        _normals[3*_trias[3*tria + 1] + 1] += n[1];
        _normals[3*_trias[3*tria + 1] + 2] += n[2];

        _normals[3*_trias[3*tria + 2] + 0] += n[0];
        _normals[3*_trias[3*tria + 2] + 1] += n[1];
        _normals[3*_trias[3*tria + 2] + 2] += n[2];
    };

    for ( int ii = 0; ii < _trias.size() / 3; ++ii )
        evalTriaNorm(ii);

    for ( int ii = 0; ii < _normals.size() / 3 ; ++ii )
    {
        float *nor = &_normals[3*ii];
        float len = std::sqrt(nor[0] * nor[0] + nor[1] * nor[1]  + nor[2] * nor[2]);
        if ( len > 0 )
        {
            nor[0] /= len;
            nor[1] /= len;
            nor[2] /= len;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::ComputeCog()
{
    float sum[3] = {0.0f, 0.0f, 0.0f};
    size_t numVertices = _vertices.size() / 3;
    for ( size_t ii = 0; ii < numVertices; ++ii )
    {
        const float* v = &_vertices[3*ii];
        sum[0] += v[0];
        sum[1] += v[1];
        sum[2] += v[2];
    }

    _cog = glm::vec3(sum[0], sum[1], sum[2]);
    _cog /= numVertices;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void mesh::ComputeBBox()
{
    _bbox = box3();
    size_t numVertices = _vertices.size() / 3;
    for ( size_t ii = 0; ii < numVertices; ++ii )
    {
        const float* v = &_vertices[3*ii];
        _bbox.expand(v);
    }
}
