#pragma once
#include "EventHandlers.h"

#include <vector>
#include <iostream>

class EventTarget
{
    // needed to extract TEvent from Lambda

    template<typename TFunctor>
    struct EventHandlerTraits;

    template<typename EventType>
    struct EventHandlerTraits<void(*)(EventType&)>
    {
        using TEvent = EventType;
    };

    template<typename ClassType, typename EventType>
    struct EventHandlerTraits<void(ClassType::*)(EventType&)>
    {
        using TClass = ClassType;
        using TEvent = EventType;
    };

    template<typename ClassType, typename EventType>
    struct EventHandlerTraits<void(ClassType::*)(EventType&) const>
    {
        using TClass = ClassType;
        using TEvent = EventType;
    };

    template<typename TFunctor>
    struct EventHandlerTraits : EventHandlerTraits<decltype(&TFunctor::operator())>
    {
    };

public:
    // Lambda and Functors handlers
    template<typename F, typename TEvent = typename EventHandlerTraits<std::decay_t<F>>::TEvent>
    void SubscribeFunctor(F&& f, const bool capture = false)
    {
        using THandler = EventFunctorHandler<F, TEvent>;

        if (!HasHandler(rtti::GetTypeId<THandler>(), capture))
        {
            auto handler = std::make_unique<THandler>(std::forward<F>(f));
            AddHandler(std::move(handler), capture);
        }
        else
        {
            std::cout << "Handler already subscribed!\n";
        }
    }
    template<typename F, typename TEvent = typename EventHandlerTraits<std::decay_t<F>>::TEvent>
    void UnsubscribeFunctor(F&& f, const bool capture = false)
    {
        using THandler = EventFunctorHandler<F, TEvent>;
        RemoveHandler(rtti::GetTypeId<THandler>(), capture);
    }

    // Function handlers
    template<typename TEvent>
    void SubscribeFunction(TFunctionPtr<TEvent> func, const bool capture = false)
    {
        using THandler = EventFunctionHandler<TEvent>;

        if (!HasHandler(rtti::GetTypeId<THandler>(), capture))
        {
            auto handler = std::make_unique<THandler>(func);
            AddHandler(std::move(handler), capture);
        }
        else
        {
            std::cout << "Handler already subscribed!\n";
        }
    }
    template<typename TEvent>
    void UnsubscribeFunction(TFunctionPtr<TEvent> func, const bool capture = false)
    {
        using THandler = EventFunctionHandler<TEvent>;
        RemoveHandler(rtti::GetTypeId<THandler>(), capture);
    }

    // Member function handlers
    template<typename TClass, typename TEvent>
    void SubscribeMethod(TClass* object, TMethodPtr<TClass, TEvent> method, const bool capture = false)
    {
        using THandler = EventMemberFunctionHandler<TClass, TEvent>;

        if (!HasHandler(rtti::GetTypeId<THandler>(), capture))
        {
            auto handler = std::make_unique<THandler>(object, method);
            AddHandler(std::move(handler), capture);
        }
        else
        {
            std::cout << "Handler already subscribed!\n";
        }
    }
    template<typename TClass, typename TEvent>
    void UnsubscribeMethod(TClass* object, TMethodPtr<TClass, TEvent> method, const bool capture = false)
    {
        using THandler = EventMemberFunctionHandler<TClass, TEvent>;
        RemoveHandler(rtti::GetTypeId<THandler>(), capture);
    }

    void UnsubscribeAll()
    {
        m_captureHandlers.clear();
        m_bubbleHandlers.clear();
    }

    template<typename TEvent>
    bool Dispatch(TEvent& ev) const
    {
        return DispatchInternalWrapper(ev, rtti::GetTypeId<TEvent>());
    }

    // return true if not stopped
    inline const EventTarget* GetParent() const { return m_parent; }
    void SetParent(const EventTarget* parent) { m_parent = parent; }

protected:
    EventTarget() = default;

private:
    std::vector<THandlerPtr>& GetHandlers(const bool capture);
    const std::vector<THandlerPtr>& GetHandlers(const bool capture) const;

    bool HasHandler(const rtti::TypeId handlerTypeId, const bool capture) const;

    void AddHandler(THandlerPtr&& handler, const bool capture);
    void RemoveHandler(const rtti::TypeId handlerTypeId, const bool capture);

    bool DispatchTarget(Event& ev, const rtti::TypeId eventTypeId, const bool capture) const;
    bool DispatchInternalWrapper(Event& ev, const rtti::TypeId eventTypeId) const;
    void DispatchInternal(Event& ev, const rtti::TypeId eventTypeId) const;

    const EventTarget* m_parent = nullptr;

    std::vector<THandlerPtr> m_captureHandlers;
    std::vector<THandlerPtr> m_bubbleHandlers;
};
