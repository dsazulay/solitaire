#pragma once

#include <cstddef>

template<typename Tag>
class Handle
{
public:
    explicit Handle(size_t id) : m_id(id) {}
    explicit operator size_t() const { return m_id; }

private:
    size_t m_id;
};
