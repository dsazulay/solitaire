#pragma once

#include <vector>
#include "moving_animation.h"
#include "dragging_animation.h"

class AnimationEngine
{
public:
    auto update() -> void;
    auto addDraggingAnimation(DraggingAnimation anim) -> void;
    auto addMovingAnimation(MovingAnimation anim) -> void;
    auto stopDraggingAnimation() -> void;
    auto getMovingAnimationQuantity() -> int;
    auto isAnyAnimationPlaying() -> bool;

private:
    std::vector<MovingAnimation> m_movingAnimation;
    DraggingAnimation m_draggingAnimation;
};

