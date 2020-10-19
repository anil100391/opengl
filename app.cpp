#include "app.h"

#include <iostream>
#include <string>

#include "events/keyevent.h"
#include "events/mouseevent.h"
#include "events/windowevent.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void KeyCallback( GLFWwindow *window,
                  int         key,
                  int         scancode,
                  int         action,
                  int         mods )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );
    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( window, GLFW_TRUE );
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
void MouseMoveCallback( GLFWwindow *window,
                        double      xpos,
                        double      ypos )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );
    MouseMoveEvent evt( (int)xpos, (int)ypos );
    app->OnEvent( evt );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MouseButtonCallback( GLFWwindow *window,
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void MouseScrollCallback( GLFWwindow *window,
                          double xoffset,
                          double yoffset )
{
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );

    MouseScrollEvent evt(xoffset, yoffset);
    app->OnEvent(evt);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void WindowResizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    WindowResizeEvent evt(width, height);
    auto app = static_cast<Application *>( glfwGetWindowUserPointer( window ) );
    app->OnEvent(evt);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
WindowProperties::WindowProperties( unsigned int w,
                                    unsigned int h,
                                    const char *title )
    : _width(w),
      _height(h),
      _title(title)
{
}

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

    // Callbacks
    glfwSetKeyCallback( _window, KeyCallback );
    glfwSetCursorPosCallback( _window, MouseMoveCallback );
    glfwSetMouseButtonCallback( _window, MouseButtonCallback );
    glfwSetScrollCallback( _window, MouseScrollCallback );
    glfwSetFramebufferSizeCallback( _window, WindowResizeCallback );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Application::~Application()
{
    if ( _window )
        glfwDestroyWindow( _window );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::Update()
{
    glfwSwapBuffers( _window );
    glfwPollEvents();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::GetWindowSize(int &width, int &height) const noexcept
{
    glfwGetWindowSize(_window, &width, &height);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float Application::GetCurrentTime() const noexcept
{
    return glfwGetTime();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::GetCursorPosition( double &x, double &y ) const noexcept
{
    glfwGetCursorPos( _window, &x, &y );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool Application::OnEvent( Event &evt )
{
    return true;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Application::Run()
{
    while ( !glfwWindowShouldClose(_window) )
    {
        Update();
    }
}
