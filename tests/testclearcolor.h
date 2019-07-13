#ifndef _testclearcolor_h_
#define _testclearcolor_h_

#include "test.h"

namespace test
{

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class TestClearColor : public Test
{
public:

    TestClearColor();
    ~TestClearColor();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

private:

    float   _clearColor[4] = {0.2f, 0.3f, 0.8f, 1.0f};
};

}

#endif // _testclearcolor_h_
