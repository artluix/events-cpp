#pragma once
#include <cstdint>

namespace rtti
{
    using TypeId = std::uintptr_t;
    //using TypeId = std::uint64_t;

    template<typename T>
    TypeId GetTypeId()
    {
        static const char s_id;
        return reinterpret_cast<std::uintptr_t>(&s_id);
    }
} // namespace rtti
