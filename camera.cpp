#include <cmath>
#include <iostream>

#include "camera.h"
#include "events/event.h"
#include "events/keyevent.h"
#include "events/mouseevent.h"

#include <GLFW/glfw3.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Camera::OnEvent( Event &evt )
{
    auto evtType = evt.GetEventType();

    static bool buttonPressed = false;
    static bool dragging = false;
    static int startDragX = 0;
    static int startDragY = 0;

    switch ( evtType )
    {
    case EventType::MouseMoved:
    {
        auto &mouseEvt = static_cast<MouseMoveEvent &>(evt);
        dragging = buttonPressed;
        if ( dragging )
        {
            const glm::vec3 &cor = GetLookAt();
            const glm::vec3 &pos = GetPosition();
            int dx = mouseEvt.X() - startDragX;
            if ( dx != 0 )
                dx = dx / std::abs( dx );
            startDragX = mouseEvt.X();
            startDragY = mouseEvt.Y();
            double dtheta = 2.0 * dx * M_PI / 180.0;
            double theta = std::atan2( pos[1], pos[0] );
            double len = std::sqrt( pos[0] * pos[0] + pos[1] * pos[1] );
            theta += dtheta;
            glm::vec3 newpos = pos;
            newpos[0] = static_cast<float>(len * std::cos( theta ));
            newpos[1] = static_cast<float>(len * std::sin( theta ));
            SetPosition( newpos );
        }
        break;
    }
    case EventType::MouseButtonPressed:
    {
        auto &mouseEvt = static_cast<MousePressedEvent &>(evt);
        if ( mouseEvt.GetButton() == MouseEvent::Button::MIDDLE )
        {
            buttonPressed = true;
            startDragX = mouseEvt.X();
            startDragY = mouseEvt.Y();
        }
        break;
    }
    case EventType::MouseButtonReleased:
    {
        auto &mouseEvt = static_cast<MouseReleasedEvent &>(evt);
        if ( mouseEvt.GetButton() == MouseEvent::Button::MIDDLE )
        {
            buttonPressed = false;
        }
        break;
    }
    case EventType::MouseScrolled:
    {
        auto &mouseEvt = static_cast<MouseScrollEvent &>(evt);
        const auto &lookAt = GetLookAt();
        const auto &eye = GetPosition();
        glm::vec3 dir = eye - lookAt;
        float scale = static_cast<float>(0.1f * mouseEvt.YOffset());
        dir = dir + dir * scale;
        SetPosition( lookAt + dir );
        break;
    }
    case EventType::KeyPressed:
    {
        auto &keyEvent = static_cast<KeyPressedEvent &>(evt);
        if ( keyEvent.GetKeyCode() == GLFW_KEY_F )
        {
            std::cout << "Fit to view... unimplemented!\n";
        }
        break;
    }
    case EventType::WindowResize:
    default: break;
    }
}
