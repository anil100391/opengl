#include "testobjloader.h"
#include "../renderer.h"
#include "../light.h"
#include "../events/event.h"
#include <imgui.h>
#include <fstream>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static std::vector<float> ComputeNormals(std::vector<float> &positions, std::vector<unsigned int> &trias)
{
    std::vector<float> normals(positions.size(), 0.0f);

    auto evalTriaNorm = [&normals, &positions, &trias](unsigned int tria)
    {
        float *v0 = &positions[3*trias[3*tria]];
        float *v1 = &positions[3*trias[3*tria + 1]];
        float *v2 = &positions[3*trias[3*tria + 2]];
        float e0[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        float e1[3] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
        float n[3] = { e0[1] * e1[2] - e1[1] * e0[2],
                       e1[0] * e0[2] - e0[0] * e1[2],
                       e0[0] * e1[1] - e1[0] * e0[1] };

        normals[3*trias[3*tria] + 0] += n[0];
        normals[3*trias[3*tria] + 1] += n[1];
        normals[3*trias[3*tria] + 2] += n[2];

        normals[3*trias[3*tria + 1] + 0] += n[0];
        normals[3*trias[3*tria + 1] + 1] += n[1];
        normals[3*trias[3*tria + 1] + 2] += n[2];

        normals[3*trias[3*tria + 2] + 0] += n[0];
        normals[3*trias[3*tria + 2] + 1] += n[1];
        normals[3*trias[3*tria + 2] + 2] += n[2];
    };

    for ( int ii = 0; ii < trias.size() / 3; ++ii )
        evalTriaNorm(ii);

    for ( int ii = 0; ii < normals.size() /3 ; ++ii )
    {
        float *nor = &normals[3*ii];
        float len = std::sqrt(nor[0] * nor[0] + nor[1] * nor[1]  + nor[2] * nor[2]);
        if ( len > 0 )
        {
            nor[0] /= len;
            nor[1] /= len;
            nor[2] /= len;
        }
    }
    return normals;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestObjLoader::TestObjLoader()
    : _mesh("res/suzanne.obj"),
      _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -300.0f))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, 0.0f, 540.0f)),
      _modelLocation(200.0f, 200.0f, 0.0f)
{
    std::vector<float> &positions = _mesh._vertices;
    std::vector<unsigned int> &trias = _mesh._trias;
    std::vector<unsigned int> &quad = _mesh._quads;;

    std::for_each(positions.begin(), positions.end(), [](float &v) { v*=100.0f; });

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    std::vector<float> normals = ComputeNormals(positions, trias);
    std::vector<float> vertices;
    for ( int ii = 0; ii < positions.size() / 3; ++ii )
    {
        // push positions
        vertices.push_back(positions[3*ii]);
        vertices.push_back(positions[3*ii+1]);
        vertices.push_back(positions[3*ii+2]);
        // push normals
        vertices.push_back(normals[3*ii]);
        vertices.push_back(normals[3*ii+1]);
        vertices.push_back(normals[3*ii+2]);
    }

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    _vao->AddBuffer(*_vbo, layout);

    _ibo = std::make_unique<IndexBuffer>(trias.data(), trias.size());

    _shader = std::make_unique<Shader>("res/shaders/obj.shader");
    _shader->Bind();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestObjLoader::~TestObjLoader()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnUpdate(float deltaTime)
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnRender()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), _modelLocation);
        model = glm::rotate(model, (float)M_PI / 2.0f, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, _modelRotation, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, glm::vec3(_modelScale));

        // set transformation matrices uniforms
        _shader->SetUniformMat4f("u_M", model);
        _shader->SetUniformMat4f("u_V", _viewMat);
        _shader->SetUniformMat4f("u_P", _projMat);

        // set material uniforms
        material m { glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
                     glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // diffuse
                     glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // specular
                     32.0f };                           // shininess

        _shader->SetUniform4f("material.ambient", m._ambient);
        _shader->SetUniform4f("material.diffuse", m._diffuse);
        _shader->SetUniform4f("material.specular", m._specular);
        _shader->SetUniform1f("material.shininess", m._shininess);

        // set light uniforms
        light l { glm::vec3(0.0f, 0.0f, 0.0f),       // position
                  glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), // ambient
                  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), // diffuse
                  glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // specular
                };

        _shader->SetUniform3f("light.position", l._position);
        _shader->SetUniform4f("light.ambient", l._ambient);
        _shader->SetUniform4f("light.diffuse", l._diffuse);
        _shader->SetUniform4f("light.specular", l._specular);

        _shader->Bind();
        renderer.Draw(*_vao, *_ibo, *_shader);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnImGuiRender()
{
    ImGui::SliderFloat("Rotation", &_modelRotation, 0.0, 2 * M_PI);
    ImGui::SliderFloat("Scale", &_modelScale, 1.0, 4.0f);
    ImGui::SliderFloat3("Location", &_modelLocation.x, 0.0, 960.0f);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestObjLoader::OnEvent( Event &evt )
{
    std::cout << evt << "\n";
}

}
