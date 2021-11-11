#ifndef _testfragmentshader_h_
#define _testfragmentshader_h_

#include "test.h"
#include "../vertexarray.h"
#include "../vertexbufferlayout.h"
#include "../vertexbuffer.h"
#include "../indexbuffer.h"
#include "../shader.h"
#include "../events/event.h"
#include "../utils/meshgl.h"

#include <memory>

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestFractal : public Test
{
public:

    TestFractal(Application *app);
    ~TestFractal();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;
    virtual void OnEvent(Event &evt) override;

private:

    void PopulateMeshBuffers();
    void Draw();

    std::unique_ptr<MeshGL>         _quadGL;
    std::unique_ptr<Shader>         _shader;

    // 2d coordinate params
    float                           _mouseX;
    float                           _mouseY;
    float                           _centerX;
    float                           _centerY;
    float                           _spanY;

    // window dimensions
    int                             _windowWidth  = 960;
    int                             _windowHeight = 540;

    // fractal mode... julia ... mandelbrot
    int                             _mode = 0;
};

}

#endif // _testfragmentshader_h_
