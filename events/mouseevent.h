#ifndef _mouse_event_h_
#define _mouse_event_h_

#include "event.h"
#include <sstream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class MouseEvent : public Event
{
public:

    enum class Button
    {
        LEFT, RIGHT, MIDDLE
    };

    inline int X() const
    {
        return _x;
    }

    inline int Y() const
    {
        return _y;
    }

    virtual int GetCategoryFlags() const override
    {
        return EC_Mouse | EC_Input;
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << GetName() << ": " << _x << ", " << _y;
        return ss.str();
    }

protected:

    MouseEvent(int x, int y)
        : _x(x), _y(y)
    {}

    int    _x;
    int    _y;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class MouseMoveEvent : public MouseEvent
{
public:
    MouseMoveEvent( int x, int y )
        : MouseEvent( x, y )
    {}

    EventType GetEventType() const override
    {
        return EventType::MouseMoved;
    }

    static EventType GetStaticEventType()
    {
        return EventType::MouseMoved;
    }

    const char* GetName() const override
    {
        return "MouseMovedEvent";
    }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class MousePressedEvent : public MouseEvent
{
public:
    MousePressedEvent( int x, int y, MouseEvent::Button button )
        : MouseEvent( x, y ), _button( button )
    {}

    EventType GetEventType() const override
    {
        return EventType::MouseButtonPressed;
    }

    static EventType GetStaticEventType()
    {
        return EventType::MouseButtonPressed;
    }

    const char* GetName() const override
    {
        return "MousePressedEvent";
    }

    virtual int GetCategoryFlags() const override
    {
        return EC_Mouse | EC_MouseButton | EC_Input;
    }

    [[nodiscard]] MouseEvent::Button GetButton() const noexcept
    {
        return _button;
    }

private:

    MouseEvent::Button  _button;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class MouseReleasedEvent : public MouseEvent
{
public:
    MouseReleasedEvent( int x, int y, MouseEvent::Button button )
        : MouseEvent( x, y ), _button( button )
    {}

    EventType GetEventType() const override
    {
        return EventType::MouseButtonReleased;
    }

    static EventType GetStaticEventType()
    {
        return EventType::MouseButtonReleased;
    }

    const char* GetName() const override
    {
        return "MouseReleasedEvent";
    }

    virtual int GetCategoryFlags() const override
    {
        return EC_Mouse | EC_MouseButton | EC_Input;
    }

    [[nodiscard]] MouseEvent::Button GetButton() const noexcept
    {
        return _button;
    }

private:

    MouseEvent::Button  _button;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class MouseScrollEvent : public MouseEvent
{
public:
    MouseScrollEvent( double xoffset, double yoffset )
        : MouseEvent( 0, 0 ),
          _xoffset(xoffset),
          _yoffset(yoffset)
    {}

    EventType GetEventType() const override
    {
        return EventType::MouseScrolled;
    }

    static EventType GetStaticEventType()
    {
        return EventType::MouseScrolled;
    }

    const char* GetName() const override
    {
        return "MouseScrolledEvent";
    }

    virtual int GetCategoryFlags() const override
    {
        return EC_Mouse | EC_MouseButton | EC_Input;
    }

    double XOffset() const noexcept
    {
        return _xoffset;
    }

    double YOffset() const noexcept
    {
        return _yoffset;
    }

private:

    double _xoffset;
    double _yoffset;
};

#endif // _mouse_event_h_
