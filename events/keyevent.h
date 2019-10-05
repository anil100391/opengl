#ifndef _key_event_h_
#define _key_event_h_

#include "event.h"
#include <sstream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class KeyEvent : public Event
{
public:
    inline int GetKeycode() const
    {
        return _keyCode;
    }

    virtual int GetCategoryFlags() const
    {
        return EC_Keyboard | EC_Input;
    }

    virtual EventType GetEventType() const = 0;

protected:

    KeyEvent(int keycode)
        : _keyCode(keycode)
    {}

    int    _keyCode;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class KeyPressedEvent : public KeyEvent
{
public:
    KeyPressedEvent( int keycode, int repeatcount)
        : KeyEvent(keycode), _repeatCount(repeatcount)
    {}

    inline int GetRepeatCount() const
    {
        return _repeatCount;
    }

    EventType GetEventType() const override
    {
        return EventType::KeyPressed;
    }

    static EventType GetStaticEventType()
    {
        return EventType::KeyPressed;
    }

    const char* GetName() const
    {
        return "KeyPressedEvent";
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << _keyCode << " (" << _repeatCount << " repeats)";
        return ss.str();
    }

private:

    int     _repeatCount = 0;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent( int keycode )
        : KeyEvent(keycode)
    {}

    EventType GetEventType() const override
    {
        return EventType::KeyReleased;
    }

    static EventType GetStaticEventType()
    {
        return EventType::KeyReleased;
    }

    const char* GetName() const
    {
        return "KeyReleasedEvent";
    }

    std::string ToString() const override
    {
        std::stringstream ss;
        ss << "KeyReleased: " << _keyCode;
        return ss.str();
    }
};


#endif // _key_event_h_
