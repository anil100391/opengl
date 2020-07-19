#include "testchess.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr float cell_size = 50.0f;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestChess::TestChess(Application *app)
    : Test(app),
      _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
{
    float cellCoords[] = { 0.0f,      0.0f,
                           cell_size, 0.0f,
                           cell_size, cell_size,
                           0.0f,      cell_size };

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

    _shader->Bind();
    for (unsigned int ii = 0; ii < 64; ++ii )
    {
        unsigned int row  = ii / 8;
        unsigned int col = ii % 8;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cell_size*row, cell_size*col, 0));
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shader->SetUniformMat4f("u_MVP", mvp);

        _shader->SetUniform1i("u_Cell", ii);

        renderer.Draw(*_vao, *_ibo, *_shader);
    }
}
}
