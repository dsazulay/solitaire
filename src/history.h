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
    void recordMove(T& t)
    {
        m_undoStack.push(t);
        if (m_redoStack.size() > 0)
            m_redoStack = std::stack<T>();
    }

    void undo()
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

    void redo()
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

    void clearStacks()
    {
        m_undoStack = std::stack<T>();
        m_redoStack = std::stack<T>();
    }

    bool isUndoStackEmpty() const
    {
        return m_undoStack.empty();
    }

    bool isRedoStackEmpty() const
    {
        return m_redoStack.empty();
    }

    T getTopUndoMove() const
    {
        return m_undoStack.top();
    }

    T getTopRedoMove() const
    {
        return m_redoStack.top();
    }

private:
    std::stack<T> m_undoStack;
    std::stack<T> m_redoStack;
};
