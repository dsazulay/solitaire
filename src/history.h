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
    void recordMove(T& t);
    void undo();
    void redo();
    void clearStacks();
    bool isUndoStackEmpty() const;
    bool isRedoStackEmpty() const;
    T getTopUndoMove() const;
    T getTopRedoMove() const;

private:
    std::stack<T> m_undoStack;
    std::stack<T> m_redoStack;
};

template<Swappable T>
void History<T>::recordMove(T& t)
{
    m_undoStack.push(t);
    if (m_redoStack.size() > 0)
        m_redoStack = std::stack<T>();
}

template<Swappable T>
void History<T>::undo()
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

template<Swappable T>
void History<T>::redo()
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

template<Swappable T>
void History<T>::clearStacks()
{
    m_undoStack = std::stack<T>();
    m_redoStack = std::stack<T>();
}

template<Swappable T>
bool History<T>::isUndoStackEmpty() const
{
    return m_undoStack.empty();
}

template<Swappable T>
bool History<T>::isRedoStackEmpty() const
{
    return m_redoStack.empty();
}

template<Swappable T>
T History<T>::getTopUndoMove() const
{
    return m_undoStack.top();
}

template<Swappable T>
T History<T>::getTopRedoMove() const
{
    return m_redoStack.top();
}
