#pragma once
#include "RTTI.h"

#include <memory>

class Event;

class EventHandlerBase
{
public:
    virtual rtti::TypeId GetTypeId() const = 0;
    bool operator==(const EventHandlerBase& other) const { return GetTypeId() == other.GetTypeId(); }

    virtual void operator()(Event& ev, const rtti::TypeId eventTypeId) const = 0;

    virtual ~EventHandlerBase() {}

protected:
    EventHandlerBase() = default;
};
using THandlerPtr = std::unique_ptr<EventHandlerBase>;

//-------------------------------------------------------------------------
template<typename TEvent>
using TFunctionPtr = void(*)(TEvent&);

template<typename TClass, typename TEvent>
using TMethodPtr = void(TClass::*)(TEvent&);

//-------------------------------------------------------------------------

template<typename TEvent>
class EventFunctionHandler : public EventHandlerBase
{
public:
    using TFunctionPtr = ::TFunctionPtr<TEvent>;

    rtti::TypeId GetTypeId() const override { return rtti::GetTypeId<EventFunctionHandler<TEvent>>(); }

    EventFunctionHandler(TFunctionPtr func) : func(func) {}

    void operator()(Event& ev, const rtti::TypeId eventTypeId) const override
    {
        if (eventTypeId == rtti::GetTypeId<TEvent>())
        {
            std::invoke(func, static_cast<TEvent&>(ev));
        }
    }

private:
    TFunctionPtr func;
};

//-------------------------------------------------------------------------

template<typename TClass, typename TEvent>
class EventMemberFunctionHandler : public EventHandlerBase
{
public:
    using TMethodPtr = ::TMethodPtr<TClass, TEvent>;

    rtti::TypeId GetTypeId() const override { return rtti::GetTypeId<EventMemberFunctionHandler<TClass, TEvent>>(); }

    EventMemberFunctionHandler(TClass* object, TMethodPtr method) : object(object), method(method) {}

    void operator()(Event& ev, const rtti::TypeId eventTypeId) const override
    {
        if (!!object && eventTypeId == rtti::GetTypeId<TEvent>())
        {
            std::invoke(method, object, static_cast<TEvent&>(ev));
        }
    }

private:
    TClass* object;
    TMethodPtr method;
};

//-------------------------------------------------------------------------

template<typename TFunctor, typename TEvent>
class EventFunctorHandler : public EventHandlerBase
{
public:
    rtti::TypeId GetTypeId() const override { return rtti::GetTypeId<EventFunctorHandler<TFunctor, TEvent>>(); }

    EventFunctorHandler(TFunctor&& functor) : functor(std::forward<TFunctor>(functor)) {}

    void operator()(Event& ev, const rtti::TypeId eventTypeId) const override
    {
        if (eventTypeId == rtti::GetTypeId<TEvent>())
        {
            std::invoke(functor, static_cast<TEvent&>(ev));
        }
    }

private:
    TFunctor functor;
};
