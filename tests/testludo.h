#ifndef _testsudo_h_
#define _testludo_h_

#include "ludo/ludo.h"
#include "test.h"
#include <memory>
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../texture.h"
#include "../utils/meshgl.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <irrKlang.h>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestLudo : public Test
{
public:

    TestLudo(Application *app);
    virtual ~TestLudo();

    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Event& evt) override;

private:

    enum class SCENE
    {
        TWOD, THREED
    };

    // Pieces
    class PieceGL : public MeshGL
    {
    public:

        enum class Type
        {
            red, blue, green, yellow
        };

        PieceGL( Type type, bool twod, TestLudo *parent );
        ~PieceGL() = default;

    private:

        void GeneratePieceGLBuffers();
        void Generate2DPieceGLBuffers();
        void Generate3DPieceGLBuffers();
        static std::array<std::array<float, 8>, 12> GetPieceTexCoordinates();

        static std::array<std::array<float, 8>, 12> s_texCoords;

        Type                            _type;
        TestLudo                      *_parent = nullptr;

        std::unique_ptr<VertexArray>    _vao;
        std::unique_ptr<VertexBuffer>   _vbo;
        std::unique_ptr<IndexBuffer>    _ibo;

        bool                            _twod = true;
    };

    friend class PieceGL;

    // generate gl buffers
    void GenerateBoardGLBuffers();
    void GeneratePieceGLBuffers();

    void Generate2DBoardGLBuffers();
    void Generate3DBoardGLBuffers();
    void Generate2DPieceGLBuffers();
    void Generate3DPieceGLBuffers();

    // Draw
    void DrawBoard();
    void Draw2DBoard();
    void Draw3DBoard();

    void DrawPieces();
    void Draw2DPieces();
    void Draw3DPieces();

    PieceGL* GetGLPiece()
    {
        return nullptr;
    }

    void MakeEngineMove();

    // board gl objects
    std::unique_ptr<MeshGL>         _boardGL;
    std::unique_ptr<Shader>         _shaderb;

    // piece gl objects
    std::vector<std::unique_ptr<PieceGL>> _pieces;
    std::unique_ptr<Shader>         _shaderp;
    std::unique_ptr<Texture>        _texturep;

    // 2d stuff
    glm::mat4                       _viewMat;
    glm::mat4                       _projMat;

    // 3d stuff
    glm::vec3                       _camPos = glm::vec3(0.0f, 0.0f, 14.0f);

    // SCENE                           _st = SCENE::THREED;
    SCENE                           _st = SCENE::TWOD;

    lboard<2u>                      _board;

    // parameters
    float                           _relativePieceSize = 1.0f;
    glm::vec4                       _darkColor = {0.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4                       _lightColor = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4                       _stopColor = { 0.8588f, 0.7058f, 0.1333f, 1.0f};
    glm::vec4                       _highlightColor = {0.75f, 0.75f, 0.25f, 1.0f};

    irrklang::ISoundEngine          *_soundEngine = nullptr;
};

}

#endif // _testchess_h_
