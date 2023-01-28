#pragma once

#include <concepts>
#include <stack>

#include "utils/log.h"

template<class T>
concept Swappable = requires(T a)
{
    { a.swap() } -> std::same_as<T>;
};

template<Swappable T>
class History
{
public:
    auto recordMove(T& t) -> void
    {
        m_undoStack.push(t);
        if (m_redoStack.size() > 0)
            m_redoStack = std::stack<T>();
    }

    auto undo() -> void
    {
        if (m_undoStack.empty())
        {
            LOG_WARN("No moves to undo");
            return;
        }

        T move = m_undoStack.top();
        T redoMove = move.swap();
        
        m_undoStack.pop();
        m_redoStack.push(redoMove);
    }

    auto redo() -> void
    {
        if (m_redoStack.empty())
        {
            LOG_WARN("No moves to redo");
            return;
        }

        T move = m_redoStack.top();
        T undoMove = move.swap();

        m_redoStack.pop();
        m_undoStack.push(undoMove);
    }

    auto clearStacks() -> void
    {
        m_undoStack = std::stack<T>();
        m_redoStack = std::stack<T>();
    }

    [[nodiscard]] auto isUndoStackEmpty() const -> bool
    {
        return m_undoStack.empty();
    }

    [[nodiscard]] auto isRedoStackEmpty() const -> bool
    {
        return m_redoStack.empty();
    }

    [[nodiscard]] auto getTopUndoMove() const -> T
    {
        return m_undoStack.top();
    }

    [[nodiscard]] auto getTopRedoMove() const -> T
    {
        return m_redoStack.top();
    }

private:
    std::stack<T> m_undoStack;
    std::stack<T> m_redoStack;
};
