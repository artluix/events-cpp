#pragma once
#include "RTTI.h"
#include "EventTarget.h"

class Event
{
    // Non-copyable
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;
    Event(Event&&) = delete;
    Event& operator=(Event&&) = delete;

    friend class EventTarget; // we can use it safely here because C++ friendship not inherited

public:
    enum class Phase : uint8_t
    {
        None = 0,
        Capture,
        Target,
        Bubble
    };

    enum EventParams : uint8_t
    {
        None = 0,
        Bubbles = 1 << 0,
        Cancellable = 1 << 1
    };

    //-------------------------------------------------------------------------

    inline bool IsBubble() const { return m_bubbles; }
    inline bool IsCancellable() const { return m_cancellable; }

    inline Phase GetPhase() const { m_phase; }

    inline bool IsPropagationStopped() const { return m_propagationStopped; }
    inline bool IsPropagationStoppedImmediately() const { return m_propagationStoppedImmediately; }
    inline bool IsDefaultPrevented() const { return m_cancellable && m_cancelled; }

    inline const EventTarget* GetTarget() const { return m_target; }
    inline const EventTarget* GetCurrentTarget() const { return m_currentTarget; }

    //-------------------------------------------------------------------------

    void PreventDefault()
    {
        m_cancelled = true;
    }

    void StopPropagation(const bool immediately = false)
    {
        m_propagationStopped = true;
        m_propagationStoppedImmediately = immediately;
    }

    explicit Event(const EventParams params = Bubbles);

private:
    Phase m_phase = Phase::None;

    bool m_bubbles : 1;
    bool m_cancellable : 1;

    bool m_cancelled : 1;
    bool m_propagationStopped : 1;
    bool m_propagationStoppedImmediately : 1;

    const EventTarget* m_target = nullptr;
    const EventTarget* m_currentTarget = nullptr;
};
