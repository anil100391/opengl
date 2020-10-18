#ifndef _app_h_
#define _app_h_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#include "events/keyevent.h"
#include "events/mouseevent.h"
#include "events/windowevent.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct WindowProperties
{
    WindowProperties() = default;
    WindowProperties(unsigned int w, unsigned int h, const char *title);
    ~WindowProperties() = default;

    unsigned int _width  = 960;
    unsigned int _height = 540;
    std::string  _title  = "DEBUG";
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class Application
{
public:
    Application( const WindowProperties &wprops = WindowProperties() );

    virtual ~Application();

    void Run();

    virtual void Update();

    void GetWindowSize(int &width, int &height) const noexcept;
    float GetCurrentTime() const noexcept;

    virtual bool OnEvent( Event &evt );

protected:

    WindowProperties _windPros;
    GLFWwindow *     _window = nullptr;
};

#endif // _app_h_
