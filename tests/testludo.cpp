#include <algorithm>
#include <map>

#include "testludo.h"
#include "../renderer.h"
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../events/keyevent.h"
#include "../events/mouseevent.h"
#include "../events/windowevent.h"

#include "../app.h"

#include "../utils/mesh.h"
#include "../utils/meshbufferobjects.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static constexpr float BOARD_SIZE = 4.0f;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestLudo::TestLudo(Application *app)
    : Test(app),
      _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
{
    if ( _st == SCENE::TWOD )
    {
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }
    else
    {
        assert( _st == SCENE::THREED );
        glEnable( GL_DEPTH_TEST );
    }

    GenerateBoardGLBuffers();
    GeneratePieceGLBuffers();

    // initialize irrKlang for audio
    _soundEngine = irrklang::createIrrKlangDevice();
    // _soundEngine->play2D("res/sounds/gong.wav", false);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestLudo::~TestLudo()
{
    _soundEngine->drop();
    _soundEngine = nullptr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::OnRender()
{
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    DrawBoard();
    DrawPieces();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::OnImGuiRender()
{
    //if ( ImGui::CollapsingHeader( "Tweaks" ) )
    {
        ImGui::SliderFloat( "Piece Size", &_relativePieceSize, 0.5f, 1.0f );
        ImGui::ColorEdit4( "Dark Square", &_darkColor[0] );
        ImGui::ColorEdit4( "Light Square", &_lightColor[0] );
        ImGui::ColorEdit4( "Stop Color", &_stopColor[0] );
        ImGui::ColorEdit4( "Highlight Square", &_highlightColor[0] );
        ImGui::SliderFloat3( "Camera", &_camPos[0], -20, 20 );
        if ( ImGui::Button( "Roll" ) )
        {
            _board.makeMove();
            _board.draw();
        }

        ImGui::Text("%d", _board.currentRolled());
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::OnEvent(Event& evt)
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
void TestLudo::GenerateBoardGLBuffers()
{
    if ( _st == SCENE::TWOD )
    {
        Generate2DBoardGLBuffers();
    }
    else
    {
        assert( _st == SCENE::THREED );
        Generate3DBoardGLBuffers();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::Generate3DBoardGLBuffers()
{
    float size = BOARD_SIZE;

    std::vector<float> vertices = { -size, -size, 0.0f, 0.0f,
                                     size, -size, 1.0f, 0.0f,
                                     size,  size, 1.0f, 1.0f,
                                    -size,  size, 0.0f, 1.0f };

    std::vector<unsigned int> conn = { 0, 1, 2,
                                       2, 3, 0 };

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);

    _boardGL = std::make_unique<MeshGL>( vertices, layout, conn );

    if ( !_shaderb )
    {
        _shaderb = std::make_unique<Shader>("res/shaders/board3d.shader");
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::Generate2DBoardGLBuffers()
{
    int w = 0, h = 0;
    _app->GetWindowSize(w, h);
    float size = std::min(w, h) / 15.0f;

    std::vector<float> vertices = { 0.0f, 0.0f,
                                    size, 0.0f,
                                    size, size,
                                    0.0f, size };

    std::vector<unsigned int> conn = { 0, 1, 2,
                                       2, 3, 0 };

    VertexBufferLayout layout;
    layout.Push<float>(2);

    _boardGL = std::make_unique<MeshGL>( vertices, layout, conn );

    if ( !_shaderb )
    {
        _shaderb = std::make_unique<Shader>("res/shaders/ludo.shader");
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::GeneratePieceGLBuffers()
{
    if ( _st == SCENE::TWOD )
    {
        Generate2DPieceGLBuffers();
    }
    else
    {
        assert( _st == SCENE::THREED );
        Generate3DPieceGLBuffers();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::Generate3DPieceGLBuffers()
{
    _pieces.clear();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::Generate2DPieceGLBuffers()
{
    _pieces.clear();

    _pieces.emplace_back( std::make_unique<PieceGL>( TestLudo::PieceGL::Type::red, _st == SCENE::TWOD, this ) );
    _pieces.emplace_back( std::make_unique<PieceGL>( TestLudo::PieceGL::Type::blue, _st == SCENE::TWOD, this ) );
    _pieces.emplace_back( std::make_unique<PieceGL>( TestLudo::PieceGL::Type::green, _st == SCENE::TWOD, this ) );
    _pieces.emplace_back( std::make_unique<PieceGL>( TestLudo::PieceGL::Type::yellow, _st == SCENE::TWOD, this ) );

    if ( !_shaderp )
    {
        _shaderp = std::make_unique<Shader>( "res/shaders/ludoPiece.shader" );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::DrawBoard()
{
    if ( _st == SCENE::TWOD )
    {
        Draw2DBoard();
    }
    else
    {
        assert( _st == SCENE::THREED );
        Draw3DBoard();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::Draw3DBoard()
{
    Renderer renderer;
    _shaderb->Bind();

    _viewMat = glm::lookAt( _camPos, glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) );

    int w = 0;
    int h = 0;
    _app->GetWindowSize( w, h );

    _projMat = glm::perspective( glm::radians( 45.0f ), (float)w / (float)h, 0.1f, 100.0f );
    // _projMat = glm::ortho( 0.0f, 1.0f * w, 0.0f, 1.0f * h, -1.0f, 1.0f );

    glm::mat4 model = glm::mat4( 1.0f );

    _shaderb->SetUniformMat4f( "u_M", model);
    _shaderb->SetUniformMat4f( "u_V", _viewMat );
    _shaderb->SetUniformMat4f( "u_P", _projMat );
    renderer.Draw( *_boardGL->vao(), *_boardGL->ibo(), *_shaderb );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::Draw2DBoard()
{
    Renderer renderer;
    _shaderb->Bind();

    int w = 0;
    int h = 0;
    _app->GetWindowSize(w, h);
    _projMat = glm::ortho(0.0f, 1.0f * w, 0.0f, 1.0f * h, -1.0f, 1.0f);

    float size = std::min(w, h) / 15.0f;

    glm::mat4 offset;
    if (w > h)
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f * (w - h), 0.0f, 0.0f));
    }
    else
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f * (h - w), 0.0f));
    }

    std::string players;
    for (unsigned int ii = 0; ii < 225; ++ii )
    {
        unsigned int row = ii / 15;
        unsigned int col = ii % 15;

        players = _board.getPlayersAtLocation( row, col );

        bool invalidSq = (players.find( '-' ) != std::string::npos);

        glm::mat4 model = glm::translate(offset, glm::vec3(size * col, size * row, 0));
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shaderb->SetUniformMat4f("u_MVP", mvp);

        _shaderb->SetUniform1i( "u_Highlighted", 0 );
        _shaderb->SetUniform1i("u_CellIndex", ii);
        _shaderb->SetUniform1i("u_CellType", invalidSq ? 0 : 1);
        _shaderb->SetUniform1f("u_Size", size);
        _shaderb->SetUniform2f("u_CellOrigin", offset[3][0], offset[3][1]);
        _shaderb->SetUniform4f("u_InvalidCellColor", _darkColor);
        _shaderb->SetUniform4f("u_RegularCellColor", _lightColor);
        _shaderb->SetUniform4f("u_StopCellColor", _stopColor);
        _shaderb->SetUniform4f("u_HighlightColor", _highlightColor);

        renderer.Draw(*_boardGL->vao(), *_boardGL->ibo(), *_shaderb);
    }

    const auto &stopSquares = _board.getStopSquares();
    for ( unsigned int stop : stopSquares )
    {
        unsigned int row = stop / 15;
        unsigned int col = stop % 15;

        players = _board.getPlayersAtLocation( row, col );

        bool invalidSq = (players.find( '-' ) != std::string::npos);
        bool xSq = (players.find( 'X' ) != std::string::npos);
        bool oSq = (players.find( 'O' ) != std::string::npos);

        glm::mat4 model = glm::translate(offset, glm::vec3(size * col, size * row, 0));
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shaderb->SetUniformMat4f("u_MVP", mvp);

        _shaderb->SetUniform1i("u_Highlighted", 0);
        _shaderb->SetUniform1i("u_CellIndex", stop);
        _shaderb->SetUniform1i("u_CellType", 2);
        _shaderb->SetUniform1f("u_Size", size);
        _shaderb->SetUniform2f("u_CellOrigin", offset[3][0], offset[3][1]);
        _shaderb->SetUniform4f("u_InvalidCellColor", _darkColor);
        _shaderb->SetUniform4f("u_RegularCellColor", _lightColor);
        _shaderb->SetUniform4f("u_StopCellColor", _stopColor);
        _shaderb->SetUniform4f("u_HighlightColor", _highlightColor);

        renderer.Draw(*_boardGL->vao(), *_boardGL->ibo(), *_shaderb);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::DrawPieces()
{
    if ( _st == SCENE::TWOD )
    {
        Draw2DPieces();
    }
    else
    {
        assert( _st == SCENE::THREED );
        Draw3DPieces();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::Draw3DPieces()
{
    // Renderer renderer;
    _shaderp->Bind();

    _viewMat = glm::lookAt( _camPos, glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) );

    int w = 0;
    int h = 0;
    _app->GetWindowSize( w, h );

    _projMat = glm::perspective( glm::radians( 45.0f ), (float)w / (float)h, 0.1f, 100.0f );
    // _projMat = glm::ortho( 0.0f, 1.0f * w, 0.0f, 1.0f * h, -1.0f, 1.0f );

    float size = 2 * BOARD_SIZE;
    float cellsize = size / 8;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::Draw2DPieces()
{
    Renderer renderer;
    _shaderp->Bind();

    int w = 0;
    int h = 0;
    _app->GetWindowSize(w, h);
    _projMat = glm::ortho(0.0f, 1.0f * w, 0.0f, 1.0f * h, -1.0f, 1.0f);

    float size = std::min(w, h) / 15.0f;

    glm::mat4 offset;
    if (w > h)
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f * (w - h), 0.0f, 0.0f));
    }
    else
    {
        offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f * (h - w), 0.0f));
    }

    // loop over pieces
    std::string players;
    for (unsigned int ii = 0; ii < 225; ++ii )
    {
        unsigned int row = ii / 15;
        unsigned int col = ii % 15;

        players = _board.getPlayersAtLocation( row, col );

        bool invalidSq = (players.find( '-' ) != std::string::npos);
        if ( invalidSq )
            continue;

        bool xSq = (players.find( 'X' ) != std::string::npos);
        bool ySq = (players.find( 'O' ) != std::string::npos);
        if ( !xSq && !ySq )
            continue;

        glm::mat4 model = glm::translate(offset, glm::vec3(size * col, size * row, 0));
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shaderp->SetUniformMat4f("u_MVP", mvp);

        _shaderp->SetUniform1i("u_CellIndex", ii);
        _shaderp->SetUniform1i("u_CellType", invalidSq ? 0 : 1);
        _shaderp->SetUniform1f("u_Size", size);
        _shaderp->SetUniform2f("u_CellOrigin", model[3][0], model[3][1]);
        _shaderp->SetUniform4f("u_Color", xSq ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        auto pgl = _pieces[xSq ? 0 : 1].get();
        renderer.Draw(*pgl->vao(), *pgl->ibo(), *_shaderp);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::MakeEngineMove()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::array<std::array<float, 8>, 12> TestLudo::PieceGL::GetPieceTexCoordinates()
{
    constexpr static float dx = 1.0f / 6;
    constexpr static float dy = 1.0f / 2;

    std::array<std::array<float, 8>, 12> data{};
    using PT = TestLudo::PieceGL::Type;
    auto to_int = []( PT pt ) { return static_cast<int>(pt); };

    /*
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
    */
    return data;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::array<std::array<float, 8>, 12> TestLudo::PieceGL::s_texCoords =
                                     TestLudo::PieceGL::GetPieceTexCoordinates();

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestLudo::PieceGL::PieceGL( Type type, bool twod, TestLudo *parent )
    : MeshGL(),
      _type( type ),
      _parent( parent ),
      _twod(twod)
{
    GeneratePieceGLBuffers();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::PieceGL::GeneratePieceGLBuffers()
{
    if ( _twod )
        Generate2DPieceGLBuffers();
    else
        Generate3DPieceGLBuffers();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::PieceGL::Generate3DPieceGLBuffers()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::PieceGL::Generate2DPieceGLBuffers()
{
    int w = 0, h = 0;
    _parent->_app->GetWindowSize(w, h);
    float size = std::min(w, h) / 15.0f;
    size *= _parent->_relativePieceSize;

    /*
    const std::array<float, 8> &texCoord = s_texCoords[static_cast<int>(_type)];

    std::vector<float> vertices = { 0.0f, 0.0f, texCoord[0], texCoord[1],
                                    size, 0.0f, texCoord[2], texCoord[3],
                                    size, size, texCoord[4], texCoord[5],
                                    0.0f, size, texCoord[6], texCoord[7] };

    std::vector<unsigned int> conn = { 0, 1, 2,
                                       2, 3, 0 };

    VertexBufferLayout layout;
    layout.Push<float>(2); // vertex
    layout.Push<float>(2); // texture coordinate
    */

    std::vector<float> vertices = { 0.0f, 0.0f,
                                    size, 0.0f,
                                    size, size,
                                    0.0f, size };

    std::vector<unsigned int> conn = { 0, 1, 2,
                                       2, 3, 0 };

    VertexBufferLayout layout;
    layout.Push<float>(2); // vertex

    PopulateBuffers( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestLudo::OnUpdate(float time)
{
}

}
