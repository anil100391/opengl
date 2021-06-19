#include "testtext.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <glm/gtc/matrix_transform.hpp>

#include "../app.h"
#include "../shader.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct Character
{
    Character( unsigned int rendererID,
               const glm::ivec2 &size,
               const glm::ivec2 &bearing,
               unsigned int advance )
        : _rendererID( rendererID ),
          _size( size ),
          _bearing( bearing ),
          _advance( advance )
    {}

    unsigned int _rendererID;  // ID handle of the glyph texture
    glm::ivec2   _size;        // Size of glyph
    glm::ivec2   _bearing;     // Offset from baseline to left/top of glyph
    unsigned int _advance;     // Offset to advance to next glyph
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::map<char, Character> Characters;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TestText::TestText( Application *app )
    : Test( app )
{
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    FT_Library ft;
    if ( FT_Init_FreeType( &ft ) )
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if ( FT_New_Face( ft, "res/fonts/trajan-pro/TrajanPro.ttf", 0, &face ) )
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes( face, 0, 72 );

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // disable byte-alignment restriction

    for ( unsigned char c = 0; c < 128; c++ )
    {
        // load character glyph
        if ( FT_Load_Char( face, c, FT_LOAD_RENDER ) )
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures( 1, &texture );
        glBindTexture( GL_TEXTURE_2D, texture );
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        // now store character for later use
        Characters.emplace( c, Character( texture,
                            glm::ivec2( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
                            glm::ivec2( face->glyph->bitmap_left, face->glyph->bitmap_top ),
                            face->glyph->advance.x ) );
    }

    FT_Done_Face( face );
    FT_Done_FreeType( ft );

    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    glBindVertexArray( VAO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 6 * 4, NULL, GL_DYNAMIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );

    _shader = std::make_unique<Shader>( "res/shaders/text.shader" );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestText::OnRender()
{
    int w, h;
    _app->GetWindowSize( w, h );
    glm::mat4 projection = glm::ortho( 0.0f, 1.0f * w, 0.0f, 1.0f * h );

    _shader->Bind();
    _shader->SetUniformMat4f( "projection", projection );

    RenderText( "Did I hear a squeak?", 540.0f, 570.0f, 1.0f, glm::vec3( 0.3, 0.7f, 0.5f ) );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestText::OnImGuiRender()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TestText::RenderText( const std::string &text, float x, float y,
                           float scale, const glm::vec3 &color )
{
    // activate corresponding render state
    glActiveTexture( GL_TEXTURE0 );
    glBindVertexArray( VAO );

    _shader->SetUniform3f( "textColor", color );

    // iterate through all characters
    for ( const auto& c: text )
    {
        auto it = Characters.find( c );
        if ( it == Characters.end() )
            continue;

        const auto &ch = it->second;

        float xpos = x + ch._bearing.x * scale;
        float ypos = y - (ch._size.y - ch._bearing.y) * scale;

        float w = ch._size.x * scale;
        float h = ch._size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture( GL_TEXTURE_2D, ch._rendererID );
        // update content of VBO memory
        glBindBuffer( GL_ARRAY_BUFFER, VBO );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        // render quad
        glDrawArrays( GL_TRIANGLES, 0, 6 );
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch._advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray( 0 );
    glBindTexture( GL_TEXTURE_2D, 0 );
}

}
