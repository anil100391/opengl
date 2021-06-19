#ifndef _test_text_h_
#define _test_text_h_

#include <memory>
#include <string>

#include "test.h"

#include <glm/glm.hpp>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Shader;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestText : public Test
{
public:

    TestText( Application *app );
    virtual ~TestText() {}

    virtual void OnRender() override;
    virtual void OnImGuiRender() override;

private:

    void RenderText( const std::string &text, float x, float y,
                     float scale, const glm::vec3 &color );

    unsigned int            VAO;
    unsigned int            VBO;
    std::unique_ptr<Shader> _shader;
};

}
#endif // _test_text_h_
