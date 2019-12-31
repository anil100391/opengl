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

