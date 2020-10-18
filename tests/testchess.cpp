#include <algorithm>
#include <map>

#include "testchess.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../events/keyevent.h"
#include "../events/mouseevent.h"
#include "../events/windowevent.h"

#include "../app.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestChess::TestChess(Application *app)
    : Test(app),
      _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
{
    const char *pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // const char *pos= "r2q1rk1/pppb1pbp/2np1np1/8/2PpP3/2N1BN1P/PP1QBPP1/R3K2R w KQ - 0 10"
    _board.setBoard( pos );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    GenerateBoardGLBuffers();
    GeneratePieceGLBuffers();
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
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );

    DrawBoard();
    DrawPieces();
    DrawCoordinates();
    DrawArrows();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnImGuiRender()
{
    ImGui::SliderFloat( "Piece Size", &_relativePieceSize, 0.5f, 1.0f );
    ImGui::ColorEdit4("Dark Square", &_darkColor[0]);
    ImGui::ColorEdit4("Light Square", &_lightColor[0]);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnEvent(Event& evt)
{
    auto evtType = evt.GetEventType();

    static bool buttonPressed = false;
    static bool dragging = false;
    static int startDragX = 0;
    static int startDragY = 0;

    static float curPieceSize = _relativePieceSize;

    switch (evtType)
    {
    case EventType::MouseMoved:
    {
        auto& mouseEvt = static_cast<MouseMoveEvent&>(evt);
        dragging = buttonPressed;
        if (dragging)
        {
            if ( curPieceSize != _relativePieceSize )
            {
                GeneratePieceGLBuffers();
                curPieceSize = _relativePieceSize;
            }
        }
        break;
    }
    case EventType::MouseButtonPressed:
    {
        auto& mouseEvt = static_cast<MousePressedEvent&>(evt);
        if (mouseEvt.GetButton() == MouseEvent::Button::LEFT)
        {
            buttonPressed = true;
            startDragX = mouseEvt.X();
            startDragY = mouseEvt.Y();
        }

        curPieceSize = _relativePieceSize;
        break;
    }
    case EventType::MouseButtonReleased:
    {
        auto& mouseEvt = static_cast<MouseReleasedEvent&>(evt);
        if (mouseEvt.GetButton() == MouseEvent::Button::LEFT)
        {
            buttonPressed = false;
        }

        if ( curPieceSize != _relativePieceSize )
        {
            GeneratePieceGLBuffers();
            curPieceSize = _relativePieceSize;
        }
        break;
    }
    case EventType::MouseScrolled:
    {
        auto& mouseEvt = static_cast<MouseScrollEvent&>(evt);
        break;
    }
    case EventType::KeyPressed:
    {
        auto& keyEvent = static_cast<KeyPressedEvent&>(evt);
        if (keyEvent.GetKeyCode() == GLFW_KEY_S)
        {
        }
        break;
    }
    case EventType::WindowResize:
    {
        GenerateBoardGLBuffers();
        GeneratePieceGLBuffers();
    }
        break;
    default: break;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::GenerateBoardGLBuffers()
{
    int w = 0, h = 0;
    _app->GetWindowSize(w, h);
    float size = std::min(w, h) / 8.0f;

    float cellCoords[] = { 0.0f, 0.0f,
                           size, 0.0f,
                           size, size,
                           0.0f, size };

    unsigned int indices[] = { 0, 1, 2,
                               2, 3, 0 };

    _vaob = std::make_unique<VertexArray>();
    _vbob = std::make_unique<VertexBuffer>(cellCoords, 4 * 2 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);

    _vaob->AddBuffer(*_vbob, layout);

    if ( !_ibob )
        _ibob = std::make_unique<IndexBuffer>(indices, 6);

    if ( !_shaderb )
    {
        _shaderb = std::make_unique<Shader>("res/shaders/chess.shader");
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::GeneratePieceGLBuffers()
{
    _pieces.clear();

    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_king, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_queen, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_bishop, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_knight, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_rook, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_pawn, this ) );

    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_king, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_queen, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_bishop, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_knight, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_rook, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_pawn, this ) );

    if ( !_shaderp )
    {
        _shaderp = std::make_unique<Shader>( "res/shaders/chessPiece.shader" );
    }

    if ( !_texturep )
    {
        _texturep = std::make_unique<Texture>( "res/textures/Chess_Pieces_Sprite.png" );
        _texturep->Bind();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::DrawBoard()
{
    Renderer renderer;
    _shaderb->Bind();

    int w = 0;
    int h = 0;
    _app->GetWindowSize(w, h);
    _projMat = glm::ortho(0.0f, 1.0f * w, 0.0f, 1.0f * h, -1.0f, 1.0f);

    float size = std::min(w, h) / 8.0f;

    glm::mat4 offset;
    if (w > h)
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f * (w - h), 0.0f, 0.0f));
    }
    else
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f * (h - w), 0.0f));
    }

    for (unsigned int ii = 0; ii < 64; ++ii )
    {
        unsigned int row = ii / 8;
        unsigned int col = ii % 8;

        glm::mat4 model = glm::translate(offset, glm::vec3(size * col, size * row, 0));
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shaderb->SetUniformMat4f("u_MVP", mvp);

        _shaderb->SetUniform1i("u_Cell", ii);
        _shaderb->SetUniform1f("u_Size", size);
        _shaderb->SetUniform2f( "u_CellOrigin", model[3][0], model[3][1] );
        _shaderb->SetUniform4f("u_Dark", _darkColor);
        _shaderb->SetUniform4f("u_Light", _lightColor);

        renderer.Draw(*_vaob, *_ibob, *_shaderb);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::DrawPieces()
{
    Renderer renderer;
    _shaderp->Bind();

    int w = 0;
    int h = 0;
    _app->GetWindowSize(w, h);
    _projMat = glm::ortho(0.0f, 1.0f * w, 0.0f, 1.0f * h, -1.0f, 1.0f);

    float size = std::min(w, h) / 8.0f;

    glm::mat4 offset;
    if (w > h)
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f * (w - h), 0.0f, 0.0f));
    }
    else
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f * (h - w), 0.0f));
    }

    for ( int square = 0; square < 64; ++square )
    {
        const cpiece& piece = _board[square];
        if ( piece.getType() == cpiece::none )
            continue;

        int row = square / 8;
        int col = square % 8;

        glm::mat4 model = glm::translate( offset, glm::vec3( size * (col + 0.5 * (1.0f - _relativePieceSize)),
                    size * (row + 0.5 * (1.0f - _relativePieceSize)), 0 ) );
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shaderp->SetUniformMat4f( "u_MVP", mvp );

        _shaderp->SetUniform1i( "u_Texture", 0 );

        PieceGL *pgl = GetGLPiece(piece.getType());
        renderer.Draw( pgl->vao(), pgl->ibo(), *_shaderp );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::DrawCoordinates()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::DrawArrows()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::array<std::array<float, 8>, 12> TestChess::PieceGL::GetPieceTexCoordinates()
{
    constexpr static float dx = 1.0f / 6;
    constexpr static float dy = 1.0f / 2;

    std::array<std::array<float, 8>, 12> data;
    using PT = TestChess::PieceGL::Type;
    auto to_int = []( PT pt ) { return static_cast<int>(pt); };

    data[to_int(PT::white_king)]   = { 0 * dx, 1 * dy, 1 * dx, 1 * dy, 1 * dx, 2 * dy, 0 * dx, 2 * dy };
    data[to_int(PT::white_queen)]  = { 1 * dx, 1 * dy, 2 * dx, 1 * dy, 2 * dx, 2 * dy, 1 * dx, 2 * dy };
    data[to_int(PT::white_bishop)] = { 2 * dx, 1 * dy, 3 * dx, 1 * dy, 3 * dx, 2 * dy, 2 * dx, 2 * dy };
    data[to_int(PT::white_knight)] = { 3 * dx, 1 * dy, 4 * dx, 1 * dy, 4 * dx, 2 * dy, 3 * dx, 2 * dy };
    data[to_int(PT::white_rook)]   = { 4 * dx, 1 * dy, 5 * dx, 1 * dy, 5 * dx, 2 * dy, 4 * dx, 2 * dy };
    data[to_int(PT::white_pawn)]   = { 5 * dx, 1 * dy, 6 * dx, 1 * dy, 6 * dx, 2 * dy, 5 * dx, 2 * dy };

    data[to_int(PT::black_king)]   = { 0 * dx, 0 * dy, 1 * dx, 0 * dy, 1 * dx, 1 * dy, 0 * dx, 1 * dy };
    data[to_int(PT::black_queen)]  = { 1 * dx, 0 * dy, 2 * dx, 0 * dy, 2 * dx, 1 * dy, 1 * dx, 1 * dy };
    data[to_int(PT::black_bishop)] = { 2 * dx, 0 * dy, 3 * dx, 0 * dy, 3 * dx, 1 * dy, 2 * dx, 1 * dy };
    data[to_int(PT::black_knight)] = { 3 * dx, 0 * dy, 4 * dx, 0 * dy, 4 * dx, 1 * dy, 3 * dx, 1 * dy };
    data[to_int(PT::black_rook)]   = { 4 * dx, 0 * dy, 5 * dx, 0 * dy, 5 * dx, 1 * dy, 4 * dx, 1 * dy };
    data[to_int(PT::black_pawn)]   = { 5 * dx, 0 * dy, 6 * dx, 0 * dy, 6 * dx, 1 * dy, 5 * dx, 1 * dy };
    return data;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::array<std::array<float, 8>, 12> TestChess::PieceGL::s_texCoords =
                                     TestChess::PieceGL::GetPieceTexCoordinates();

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestChess::PieceGL::PieceGL( Type type, TestChess *parent )
    : _type( type ),
      _parent( parent )
{
    GeneratePieceGLBuffers();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::PieceGL::GeneratePieceGLBuffers()
{
    int w = 0, h = 0;
    _parent->_app->GetWindowSize(w, h);
    float size = std::min(w, h) / 8.0f;
    size *= _parent->_relativePieceSize;

    const std::array<float, 8> &texCoord = s_texCoords[static_cast<int>(_type)];

    float cellCoords[] = { 0.0f, 0.0f, texCoord[0], texCoord[1],
                           size, 0.0f, texCoord[2], texCoord[3],
                           size, size, texCoord[4], texCoord[5],
                           0.0f, size, texCoord[6], texCoord[7] };

    unsigned int indices[] = { 0, 1, 2,
                               2, 3, 0 };

    _vao = std::make_unique<VertexArray>();
    _vbo = std::make_unique<VertexBuffer>(cellCoords, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2); // vertex
    layout.Push<float>(2); // texture coordinate

    _vao->AddBuffer(*_vbo, layout);

    if ( !_ibo )
        _ibo = std::make_unique<IndexBuffer>(indices, 6);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnUpdate(float time)
{
    static float lastMoveTime = time;
    if ( time > lastMoveTime + 2 )
    {
        auto moves = _board.generateMoves();

        float r = (1.0f * rand()) / RAND_MAX;
        size_t randomMove = static_cast<size_t>(r * moves.size());
        randomMove = std::clamp( randomMove, size_t(0ull), size_t(moves.size() - 1) );

        if ( !moves.empty() )
            _board.makeMove( moves.at( randomMove ) );
        lastMoveTime = time;
    }
}

}
