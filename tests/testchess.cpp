#include "testchess.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestChess::TestChess()
    : _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
{
    float cellCoords[] = { 0.0f, 0.0f,
                           100.0f, 0.0f,
                           100.0f, 100.0f,
                           0.0f, 100.0f };

    unsigned int indices[] = { 0, 1, 2,
                               2, 3, 0 };

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(cellCoords, 4 * 2 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);

    _vao->AddBuffer(*_vbo, layout);

    _ibo = std::make_unique<IndexBuffer>(indices, 6);

    _shader = std::make_unique<Shader>("res/shaders/chess.shader");
    _shader->Bind();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestChess::~TestChess()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnRender()
{
    Renderer renderer;
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear(GL_COLOR_BUFFER_BIT);

    for (unsigned int ii = 0; ii < 64; ++ii )
    {
        unsigned int row  = ii / 8;
        unsigned int col = ii % 8;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(100*row, 100*col, 0));
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shader->SetUniformMat4f("u_MVP", mvp);

        _shader->SetUniform1i("u_Cell", ii);

        _shader->Bind();
        renderer.Draw(*_vao, *_ibo, *_shader);
    }
}
}
