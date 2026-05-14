#pragma once

#include "types.h"

template<typename Tag>
class Handle
{
public:
    Handle() : m_id() {}
    explicit Handle(u64 id) : m_id(id) {}
    Handle(const Handle& handle) = default;
    auto operator=(const Handle& handle) -> Handle& = default;
    explicit operator u64() const { return m_id; }

private:
    u64 m_id;
};
