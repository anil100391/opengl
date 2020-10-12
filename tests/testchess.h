#ifndef _testchess_h_
#define _testchess_h_

#include <array>
#include "test.h"
#include <memory>
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestChess : public Test
{
public:

    TestChess(Application *app);
    virtual ~TestChess();

    virtual void OnUpdate(float deltaTime) override {}
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& evt) override;

private:

    // Pieces
    class PieceGL
    {
    public:

        enum class Type
        {
            white_king, white_queen, white_bishop, white_knight, white_rook, white_pawn,
            black_king, black_queen, black_bishop, black_knight, black_rook, black_pawn
        };

        PieceGL( Type type, TestChess *parent );
        ~PieceGL() = default;

        const VertexArray &vao() const { return *_vao; }
        const VertexBuffer &vbo() const { return *_vbo; }
        const IndexBuffer &ibo() const { return *_ibo; }

    private:

        void GeneratePieceGLBuffers();
        static std::array<std::array<float, 8>, 12> GetPieceTexCoordinates();

        static std::array<std::array<float, 8>, 12> s_texCoords;

        Type                            _type;
        TestChess                      *_parent = nullptr;

        std::unique_ptr<VertexArray>    _vao;
        std::unique_ptr<VertexBuffer>   _vbo;
        std::unique_ptr<IndexBuffer>    _ibo;
    };

    friend class PieceGL;

    // generate gl buffers
    void GenerateBoardGLBuffers();
    void GeneratePieceGLBuffers();

    // Draw
    void DrawBoard();
    void DrawPieces();
    void DrawCoordinates();
    void DrawArrows();

    // board gl objects
    std::unique_ptr<VertexArray>    _vaob;
    std::unique_ptr<VertexBuffer>   _vbob;
    std::unique_ptr<IndexBuffer>    _ibob;
    std::unique_ptr<Shader>         _shaderb;

    // piece gl objects
    std::vector<std::unique_ptr<PieceGL>> _pieces;
    std::unique_ptr<Shader>         _shaderp;
    std::unique_ptr<Texture>        _texturep;

    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;

    // parameters
    float                           _relativePieceSize = 0.95f;
};

}

#endif // _testchess_h_
