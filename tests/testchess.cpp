#include <algorithm>
#include <map>
#include <ctime>

#include "testchess.h"
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
TestChess::TestChess(Application *app)
    : Test(app),
      _viewMat(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
      _projMat(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f))
{
    const char *pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // const char *pos= "r2q1rk1/pppb1pbp/2np1np1/8/2PpP3/2N1BN1P/PP1QBPP1/R3K2R w KQ - 0 10"
    _board.setBoard( pos );

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
TestChess::~TestChess()
{
    _soundEngine->drop();
    _soundEngine = nullptr;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnRender()
{
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    DrawBoard();
    DrawPieces();
    DrawCoordinates();
    DrawArrows();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnImGuiRender()
{
    if ( ImGui::CollapsingHeader( "Tweaks" ) )
    {
        ImGui::SliderFloat( "Piece Size", &_relativePieceSize, 0.5f, 1.0f );
        ImGui::ColorEdit4( "Dark Square", &_darkColor[0] );
        ImGui::ColorEdit4( "Light Square", &_lightColor[0] );
        ImGui::ColorEdit4( "Highlight Square", &_highlightColor[0] );
        ImGui::SliderFloat3( "Camera", &_camPos[0], -20, 20 );
    }

    ImGui::SetNextItemOpen( true );
    if ( ImGui::CollapsingHeader( "Moves" ) )
    {
        for ( int ii = 0; ii <= _latestPly; ++ii )
        {
            const auto &state = _board.getBoardStateAt( ii );
            std::string moveStr = state.toString();
            if ( ii % 2 == 0 )
                ImGui::Bullet();

            // ImGui::SmallButton( moveStr.c_str(), ImVec2( 32, 20 ) );
            // ImGui::PushID( ii );
            // ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4)ImColor::HSV( i / 7.0f, 0.6f, 0.6f ) );
            // ImGui::PushStyleColor( ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV( i / 7.0f, 0.7f, 0.7f ) );
            // ImGui::PushStyleColor( ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV( i / 7.0f, 0.8f, 0.8f ) );
            if ( ImGui::Button( moveStr.c_str(), ImVec2(32, 20) ) )
            {
                if ( ii < _latestPly )
                    _board.goToPly( ii + 1 );
                else
                {
                    _board.goToPly( ii );
                    _board.makeMove( _board.getBoardStateAt( ii )._movePlayed );
                }
                _currentPly = ii;
            }

            if ( ii % 2 == 0 )
                ImGui::SameLine();

            // ImGui::PopStyleColor( 3 );
            // ImGui::PopID();
        }
    }
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
            _pickStartSq = SquareAt( mouseEvt.X(), mouseEvt.Y() );
            if ( _pickStartSq >= 0 && _pickStartSq < 64 )
            {
                const cpiece &p = _board[_pickStartSq];
                _draggingPiece = GetGLPiece( p.getType() );
                _draggingPieceLegalMoves = _board.generateMoves();
                auto it = std::remove_if( _draggingPieceLegalMoves.begin(),
                                          _draggingPieceLegalMoves.end(),
                                          [this](const cmove& m)
                                          {
                                              return (m.getfromSq() != _pickStartSq);
                                          } );
                _draggingPieceLegalMoves.erase( it, _draggingPieceLegalMoves.end() );
            }
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
            if ( _pickStartSq != -1 )
            {
                int dropSq = SquareAt( mouseEvt.X(), mouseEvt.Y() );
                if ( dropSq != -1 )
                {
                    if ( _board.makeMove( _pickStartSq, dropSq ) )
                    {
                        ++_currentPly;
                        ++_latestPly;

                        if ( _board.isInCheck(_board.sideToMove() ) )
                            _soundEngine->play2D("res/sounds/cymbal.wav", false);
                        else
                            _soundEngine->play2D("res/sounds/woodthunk.wav", false);
                        _lastMoveTime = _app->GetCurrentTime();
                        _engineTurn = true;
                    }
                }
            }
            _pickStartSq = -1;
            _draggingPieceLegalMoves.clear();
            _draggingPiece = nullptr;
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
void TestChess::Generate3DBoardGLBuffers()
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
void TestChess::Generate2DBoardGLBuffers()
{
    int w = 0, h = 0;
    _app->GetWindowSize(w, h);
    float size = std::min(w, h) / 8.0f;

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
        _shaderb = std::make_unique<Shader>("res/shaders/chess.shader");
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::GeneratePieceGLBuffers()
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
void TestChess::Generate3DPieceGLBuffers()
{
    _pieces.clear();

    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_king, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_queen, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_bishop, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_knight, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_rook, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_pawn, _st == SCENE::TWOD, this ) );

    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_king, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_queen, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_bishop, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_knight, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_rook, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_pawn, _st == SCENE::TWOD, this ) );

    if ( !_shaderp )
    {
        _shaderp = std::make_unique<Shader>( "res/shaders/piece3d.shader" );
    }

    if ( !_texturep )
    {
        // _texturep = std::make_unique<Texture>( "res/textures/Chess_Pieces_Sprite.png" );
        // _texturep->Bind();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::Generate2DPieceGLBuffers()
{
    _pieces.clear();

    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_king, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_queen, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_bishop, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_knight, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_rook, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::white_pawn, _st == SCENE::TWOD, this ) );

    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_king, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_queen, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_bishop, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_knight, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_rook, _st == SCENE::TWOD, this ) );
    _pieces.push_back( std::make_unique<PieceGL>( TestChess::PieceGL::Type::black_pawn, _st == SCENE::TWOD, this ) );

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
void TestChess::Draw3DBoard()
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
void TestChess::Draw2DBoard()
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

    std::vector<unsigned int> highlightedSq;
    highlightedSq.reserve( 64 );

    if ( _currentPly != -1 && !_draggingPiece )
    {
        const auto &state = _board.getBoardStateAt( _currentPly );
        const auto &move = state._movePlayed;
        highlightedSq.push_back( move.getfromSq() );
        highlightedSq.push_back( move.gettoSq() );
    }

    for ( const auto &move : _draggingPieceLegalMoves )
    {
        highlightedSq.push_back( move.gettoSq() );
    }

    for (unsigned int ii = 0; ii < 64; ++ii )
    {
        unsigned int row = ii / 8;
        unsigned int col = ii % 8;

        glm::mat4 model = glm::translate(offset, glm::vec3(size * col, size * row, 0));
        glm::mat4 mvp = _projMat * _viewMat * model;
        _shaderb->SetUniformMat4f("u_MVP", mvp);

        bool highlighted = std::find( highlightedSq.begin(), highlightedSq.end(), ii ) != highlightedSq.end();
        _shaderb->SetUniform1i("u_Highlighted",  highlighted ? 1 : 0);
        _shaderb->SetUniform1i("u_Cell", ii);
        _shaderb->SetUniform1f("u_Size", size);
        _shaderb->SetUniform2f("u_CellOrigin", model[3][0], model[3][1]);
        _shaderb->SetUniform4f("u_Dark", _darkColor);
        _shaderb->SetUniform4f("u_Light", _lightColor);
        _shaderb->SetUniform4f("u_Highlight", _highlightColor);

        renderer.Draw(*_boardGL->vao(), *_boardGL->ibo(), *_shaderb);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::DrawPieces()
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
void TestChess::Draw3DPieces()
{
    Renderer renderer;
    _shaderp->Bind();

    _viewMat = glm::lookAt( _camPos, glm::vec3(0, 0, 0), glm::vec3( 0, 1, 0 ) );

    int w = 0;
    int h = 0;
    _app->GetWindowSize( w, h );

    _projMat = glm::perspective( glm::radians( 45.0f ), (float)w / (float)h, 0.1f, 100.0f );
    // _projMat = glm::ortho( 0.0f, 1.0f * w, 0.0f, 1.0f * h, -1.0f, 1.0f );

    float size = 2 * BOARD_SIZE;
    float cellsize = size / 8;

    for ( int square = 0; square < 64; ++square )
    {
        const cpiece& piece = _board[square];
        if ( piece.getType() == cpiece::none )
            continue;

        int row = square / 8;
        int col = square % 8;

        float dx = - size / 2 + cellsize / 2;

        glm::mat4 model = glm::mat4( 1.0f );
        model = glm::translate( model, glm::vec3( dx, dx, 0.0f ) );
        model = glm::translate( model, glm::vec3( col * cellsize, row * cellsize, 0.0f ) );
        model = glm::rotate( model, glm::radians( 90.0f ), glm::vec3( 0.0f, 0.0f, 1.0f ) );

        _shaderp->SetUniformMat4f( "u_M", model );
        _shaderp->SetUniformMat4f( "u_V", _viewMat );
        _shaderp->SetUniformMat4f( "u_P", _projMat );

        _shaderp->SetUniform1i( "u_PieceColor", piece.getColor() == dark ? 0 : 1 );

        PieceGL* pgl = GetGLPiece( piece.getType() );
        renderer.Draw( *pgl->vao(), *pgl->ibo(), *_shaderp );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::Draw2DPieces()
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

        if ( _pickStartSq == square && _draggingPiece )
        {
            // to be drawn in the end
            continue;
        }

        glm::mat4 mvp = _projMat * _viewMat * model;
        _shaderp->SetUniformMat4f( "u_MVP", mvp );

        _shaderp->SetUniform1i( "u_Texture", 0 );

        PieceGL *pgl = GetGLPiece(piece.getType());
        renderer.Draw( *pgl->vao(), *pgl->ibo(), *_shaderp );
    }


    if ( _draggingPiece )
    {
        double mouseX = 0.0, mouseY = 0.0;
        _app->GetCursorPosition( mouseX, mouseY );
        glm::mat4 model = glm::translate( glm::mat4( 1.0f ), glm::vec3( mouseX - 0.5 * size, h - mouseY - 0.5 * size, 0.0 ) );

        glm::mat4 mvp = _projMat * _viewMat * model;
        _shaderp->SetUniformMat4f( "u_MVP", mvp );

        _shaderp->SetUniform1i( "u_Texture", 0 );

        renderer.Draw( *_draggingPiece->vao(), *_draggingPiece->ibo(), *_shaderp );
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
void TestChess::MakeEngineMove()
{
    auto moves = _board.generateMoves();

    static bool rngInit = false;
    if ( !rngInit )
    {
        rngInit = true;
        srand( time( 0 ) );
    }

    float r = (1.0f * rand()) / RAND_MAX;
    size_t randomMove = static_cast<size_t>(r * moves.size());
    randomMove = std::clamp( randomMove, size_t(0ull), size_t(moves.size() - 1 ));

    if ( !moves.empty() )
    {
        _lastMoveTime = _app->GetCurrentTime();
        _board.makeMove(  moves.at( randomMove ) );

        ++_currentPly;
        ++_latestPly;

        if ( _board.isInCheck(_board.sideToMove() ) )
            _soundEngine->play2D("res/sounds/cymbal.wav", false);
        else
            _soundEngine->play2D("res/sounds/woodthunk.wav", false);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int TestChess::SquareAt( int x, int y ) const
{
    int w = 0;
    int h = 0;
    _app->GetWindowSize(w, h);

    y = h - y;

    float size = std::min(w, h) / 8.0f;

    float xoffset = 0;
    float yoffset = 0;

    if (w > h)
        xoffset = 0.5f * (w - h);
    else
        yoffset = 0.5f * (h - w);

    int rank = (y - yoffset) / size;
    int file = (x - xoffset) / size;

    if ( rank < 0 || rank >= 8 || file < 0 || file >= 8 )
        return -1;

    int sq = rank * 8 + file;
    if ( sq >= 0 && sq < 64 )
        return sq;

    return -1;
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
TestChess::PieceGL::PieceGL( Type type, bool twod, TestChess *parent )
    : MeshGL(),
      _type( type ),
      _parent( parent ),
      _twod(twod)
{
    GeneratePieceGLBuffers();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::PieceGL::GeneratePieceGLBuffers()
{
    if ( _twod )
        Generate2DPieceGLBuffers();
    else
        Generate3DPieceGLBuffers();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::PieceGL::Generate3DPieceGLBuffers()
{
    std::unique_ptr<mesh> m = nullptr;
    switch ( _type )
    {
    case Type::black_rook:
    case Type::white_rook:
        m = std::make_unique<mesh>( "res/chess/rook.obj" );
        break;
    case Type::black_knight:
    case Type::white_knight:
        m = std::make_unique<mesh>( "res/chess/knight.obj" );
        break;
    case Type::black_bishop:
    case Type::white_bishop:
        m = std::make_unique<mesh>( "res/chess/bishop.obj" );
        break;
    case Type::black_queen:
    case Type::white_queen:
        m = std::make_unique<mesh>( "res/chess/queen.obj" );
        break;
    case Type::black_king:
    case Type::white_king:
        m = std::make_unique<mesh>( "res/chess/king.obj" );
        break;
    case Type::black_pawn:
    case Type::white_pawn:
        m = std::make_unique<mesh>( "res/chess/pawn.obj" );
        break;
    }

    assert( m != nullptr );
    std::vector<float> vertices = mbos::vbo( *m, false );
    std::vector<unsigned int> conn = mbos::ibo( *m, false );

    VertexBufferLayout layout;
    layout.Push<float>(3); // position
    layout.Push<float>(3); // normal
    layout.Push<float>(2); // texture coordinate

    PopulateBuffers( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::PieceGL::Generate2DPieceGLBuffers()
{
    int w = 0, h = 0;
    _parent->_app->GetWindowSize(w, h);
    float size = std::min(w, h) / 8.0f;
    size *= _parent->_relativePieceSize;

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

    PopulateBuffers( vertices, layout, conn );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestChess::OnUpdate(float time)
{
    if ( _engineTurn && time > _lastMoveTime + 1 )
    {
        MakeEngineMove();
        _engineTurn = false;
    }
}

}
