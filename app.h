#ifndef _app_h_
#define _app_h_
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <string>

#include "events/keyevent.h"
#include "events/mouseevent.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct WindowProperties
{
    WindowProperties() = default;

    WindowProperties( unsigned int w, unsigned int h, const char *title )
        : _width( w ), _height( h ), _title( title )
    {
    }

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

    virtual ~Application()
    {
        if ( _window )
            glfwDestroyWindow( _window );
    }

    void Run();

    virtual void Update()
    {
        glfwSwapBuffers( _window );
        glfwPollEvents();
    }

    void SetEventHandler( std::function<bool( Event &evt )> evtHandler )
    {
        _evtHandler = evtHandler;
    }

    void OnEvent( Event &evt )
    {
        if ( _evtHandler )
            _evtHandler( evt );
    }

    bool _terminate = false;

protected:
    std::function<bool( Event &evt )> _evtHandler;

    WindowProperties _windPros;
    GLFWwindow *     _window = nullptr;

private:
    static void KeyCallback( GLFWwindow *window,
                             int         key,
                             int         scancode,
                             int         action,
                             int         mods );

    static void MouseMoveCallback( GLFWwindow *window,
                                   double xpos,
                                   double ypos );

    static void MouseButtonCallback( GLFWwindow *window,
                                     int         button,
                                     int         action,
                                     int         mods );
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Application::Application( const WindowProperties &wprops )
{
    if ( !glfwInit() )
        return;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    _window = glfwCreateWindow( wprops._width,
                                wprops._height,
                                wprops._title.c_str(),
                                nullptr,
                                nullptr );

    if ( !_window )
    {
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer( _window, this );
    glfwSwapInterval( 1 );
    glfwMakeContextCurrent( _window );

    if ( GLEW_OK == glewInit() )
        std::cout << glGetString( GL_VERSION ) << std::endl;
    else
        std::cout << "Error initializing opengl context" << std::endl;

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Callbacks
    glfwSetKeyCallback( _window, KeyCallback );
    glfwSetCursorPosCallback( _window, MouseMoveCallback );
    glfwSetMouseButtonCallback( _window, MouseButtonCallback );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::Run()
{
    while ( !_terminate )
    {
        Update();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::KeyCallback( GLFWwindow *window,
                               int         key,
                               int         scancode,
                               int         action,
                               int         mods )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( window, GLFW_TRUE );
        app->_terminate = true;
    }

    static unsigned int repeatcnt = 0;

    if ( action == GLFW_PRESS )
    {
        repeatcnt = 0;
        KeyPressedEvent evt( key, 0 );
        app->OnEvent( evt );
    }
    else if ( action == GLFW_RELEASE )
    {
        KeyReleasedEvent evt( key );
        app->OnEvent( evt );
    }
    else if ( action == GLFW_REPEAT )
    {
        ++repeatcnt;
        KeyPressedEvent evt( key, repeatcnt );
        app->OnEvent( evt );
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::MouseMoveCallback( GLFWwindow *window,
                                     double      xpos,
                                     double      ypos )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );
    MouseMoveEvent evt( (int)xpos, (int)ypos );
    app->OnEvent( evt );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::MouseButtonCallback( GLFWwindow *window,
                                       int         button,
                                       int         action,
                                       int         mods )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );

    double xpos = 0.0;
    double ypos = 0.0;
    glfwGetCursorPos( window, &xpos, &ypos );

    if ( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS )
    {
        MousePressedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::RIGHT );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE )
    {
        MouseReleasedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::RIGHT );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS )
    {
        MousePressedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::LEFT );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE )
    {
        MouseReleasedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::LEFT );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS )
    {
        MousePressedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::MIDDLE );
        app->OnEvent( evt );
    }
    else if ( button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE )
    {
        MouseReleasedEvent evt( (int)xpos,
                               (int)ypos, MouseEvent::Button::MIDDLE );
        app->OnEvent( evt );
    }
}

#endif // _app_h_
