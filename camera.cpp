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
    // https://learnopengl.com/Getting-started/Camera
    auto evtType = evt.GetEventType();

    static bool buttonPressed = false;
    static bool dragging = false;
    static int prevDragX = 0;
    static int prevDragY = 0;

    switch ( evtType )
    {
    case EventType::MouseMoved:
    {
        auto &mouseEvt = static_cast<MouseMoveEvent &>(evt);
        dragging = buttonPressed;
        if ( dragging )
        {
            // keep the camera centerd at look at and rotate it
            // 1. about upvec for x delta and
            const glm::vec3 &cor = GetLookAt();
            const glm::vec3 &pos = GetPosition();

            float dist = glm::length( GetPosition() - GetLookAt() );
            glm::vec3 cameraDirection = glm::normalize( GetPosition() - GetLookAt() );
            glm::vec3 cameraRight = glm::normalize( glm::cross( GetUpVec(), cameraDirection ) );
            glm::vec3 cameraUp = glm::cross( cameraDirection, cameraRight );

            int xPixelMove = mouseEvt.X() - prevDragX;
            double dthetaX = -1.0 * xPixelMove * (1.0 / 5.0) * (M_PI / 180.0); // 1 deg rotation per 5 pixel move
            int yPixelMove = mouseEvt.Y() - prevDragY;
            double dthetaY = -1.0 * yPixelMove * (1.0 / 5.0) * (M_PI / 180.0); // 1 deg rotation per 5 pixel move

            glm::mat4 identity(1.0f);
            glm::mat4 r = glm::rotate( identity, static_cast<float>(dthetaX), cameraUp );
            r = glm::rotate( r, static_cast<float>(dthetaY), cameraRight );
            glm::vec4 newDir = r * glm::vec4( cameraDirection, 1.0f );

            SetPosition( cor + dist * glm::vec3( newDir[0], newDir[1], newDir[2] ) );

            prevDragX = mouseEvt.X();
            prevDragY = mouseEvt.Y();
        }
        break;
    }
    case EventType::MouseButtonPressed:
    {
        auto &mouseEvt = static_cast<MousePressedEvent &>(evt);
        if ( mouseEvt.GetButton() == MouseEvent::Button::MIDDLE )
        {
            buttonPressed = true;
            prevDragX = mouseEvt.X();
            prevDragY = mouseEvt.Y();
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
        const float cameraSpeed = 0.5f;
        glm::vec3 cameraPos = GetPosition();
        glm::vec3 cameraFront = glm::normalize( GetLookAt() - GetPosition() );
        glm::vec3 cameraUp = GetUpVec();

        if ( keyEvent.GetKeyCode() == GLFW_KEY_W )
        {
            cameraPos += cameraSpeed * cameraFront;
        }
        else if ( keyEvent.GetKeyCode() == GLFW_KEY_S )
        {
            cameraPos -= cameraSpeed * cameraFront;
        }
        else if ( keyEvent.GetKeyCode() == GLFW_KEY_A )
        {
            cameraPos -= glm::normalize( glm::cross( cameraFront, cameraUp ) ) * cameraSpeed;
        }
        else if ( keyEvent.GetKeyCode() == GLFW_KEY_D )
        {
            cameraPos += glm::normalize( glm::cross( cameraFront, cameraUp ) ) * cameraSpeed;
        }

        SetPosition( cameraPos );

        glm::vec3 newLookAt = cameraPos + cameraFront;
        SetLookAt( newLookAt );

        break;
    }
    case EventType::WindowResize:
    default: break;
    }
}
