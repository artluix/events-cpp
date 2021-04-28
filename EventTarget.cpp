#include "EventTarget.h"

#include "Event.h"

//-------------------------------------------------------------------------

std::vector<THandlerPtr>& EventTarget::GetHandlers(const bool capture)
{
	return capture ? m_captureHandlers : m_bubbleHandlers;
}

const std::vector<THandlerPtr>& EventTarget::GetHandlers(const bool capture) const
{
	return capture ? m_captureHandlers : m_bubbleHandlers;
}

//-------------------------------------------------------------------------

bool EventTarget::HasHandler(const rtti::TypeId handlerTypeId, const bool capture) const
{
	auto& handlers = GetHandlers(capture);

	auto it = std::find_if(handlers.begin(), handlers.end(), [handlerTypeId](const THandlerPtr& h) { return h->GetTypeId() == handlerTypeId; });
	return it != handlers.end();
}

void EventTarget::AddHandler(THandlerPtr&& handler, const bool capture)
{
	auto& handlers = GetHandlers(capture);
	handlers.push_back(std::move(handler));
}

void EventTarget::RemoveHandler(const rtti::TypeId handlerTypeId, const bool capture)
{
	auto& handlers = GetHandlers(capture);
	auto it = std::find_if(handlers.begin(), handlers.end(), [handlerTypeId](const THandlerPtr& h) { return h->GetTypeId() == handlerTypeId; });

	if (it != handlers.end())
	{
		handlers.erase(it);
	}
	else
	{
		std::cout << "Handler not found!\n";
	}
}

//-------------------------------------------------------------------------

bool EventTarget::DispatchTarget(Event& ev, const rtti::TypeId eventTypeId, const bool capture) const
{
	ev.m_currentTarget = this;

	auto& handlers = GetHandlers(capture);
	for (const auto& handlerPtr : handlers)
	{
		(*handlerPtr)(ev, eventTypeId);

		if (ev.IsPropagationStoppedImmediately())
			return false;
	}

	return !ev.IsPropagationStopped();
}

//-------------------------------------------------------------------------

bool EventTarget::DispatchInternalWrapper(Event& ev, const rtti::TypeId eventTypeId) const
{
	DispatchInternal(ev, eventTypeId);
	return !ev.IsDefaultPrevented();
}

//-------------------------------------------------------------------------

void EventTarget::DispatchInternal(Event& ev, const rtti::TypeId eventTypeId) const
{
	std::vector<const EventTarget*> targets; // TODO: check whether this should be placed here

	ev.m_target = this; // set event target here

	// 1. Create path
	{
		const EventTarget* currentTarget = this;
		while (currentTarget = currentTarget->GetParent())
		{
			targets.push_back(currentTarget);
		}
	}

	const std::size_t targetsCount = targets.size();

	// 2. Capture phase
	if (targetsCount > 0)
	{
		ev.m_phase = Event::Phase::Capture;

		for (auto it = targets.rbegin(); it != targets.rend(); it++)
		{
			const EventTarget* currentTarget = *it;
			ev.m_currentTarget = currentTarget;
			if (!currentTarget->DispatchTarget(ev, eventTypeId, true))
				return;
		}
	}

	// 3. Target phase
	{
		ev.m_phase = Event::Phase::Target;

		DispatchTarget(ev, eventTypeId, true);
		if (ev.IsPropagationStoppedImmediately())
			return;

		if (!DispatchTarget(ev, eventTypeId, false))
			return;
	}

	// 4. Bubble phase
	if (ev.IsBubble() && targetsCount > 0)
	{
		ev.m_phase = Event::Phase::Bubble;

		for (const auto& target : targets)
		{
			ev.m_currentTarget = target;
			if (!target->DispatchTarget(ev, eventTypeId, false))
				return;
		}
	}
}
