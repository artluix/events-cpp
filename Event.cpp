#include "Event.h"

Event::Event(const EventParams params /*= Bubbles*/)
	: m_bubbles(params& Bubbles)
	, m_cancellable(params& Cancellable)
	, m_cancelled(false)
	, m_propagationStopped(false)
	, m_propagationStoppedImmediately(false)
{
}
