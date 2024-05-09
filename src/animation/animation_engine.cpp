#include "animation_engine.h"

auto AnimationEngine::update() -> void
{
    auto it = m_movingAnimation.begin();
    if (it != m_movingAnimation.end())
    {
        it->update();
        if (it->isDone())
            m_movingAnimation.erase(it);
    }
    if (!m_draggingAnimation.isDone())
        m_draggingAnimation.update();
}

auto AnimationEngine::addDraggingAnimation(DraggingAnimation anim) -> void
{
    m_draggingAnimation = anim;
    m_draggingAnimation.start();
}

auto AnimationEngine::addMovingAnimation(MovingAnimation anim) -> void
{
    m_movingAnimation.push_back(anim);
}

auto AnimationEngine::stopDraggingAnimation() -> void
{
    if (!m_draggingAnimation.isDone())
        m_draggingAnimation.stop();
}

auto AnimationEngine::getMovingAnimationQuantity() -> int
{
    return static_cast<int>(m_movingAnimation.size());
}

auto AnimationEngine::isAnyAnimationPlaying() -> bool
{
    return m_movingAnimation.size() > 0 || !m_draggingAnimation.isDone();
}
