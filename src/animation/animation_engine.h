#pragma once

#include "../utils/types.h"
#include "moving_animation.h"
#include "dragging_animation.h"

#include <vector>

class AnimationEngine
{
public:
    auto update() -> void;
    auto addDraggingAnimation(DraggingAnimation anim) -> void;
    auto addMovingAnimation(MovingAnimation anim) -> void;
    auto stopDraggingAnimation() -> void;
    auto getMovingAnimationQuantity() -> i32;
    auto isAnyAnimationPlaying() -> bool;

private:
    std::vector<MovingAnimation> m_movingAnimation;
    DraggingAnimation m_draggingAnimation;
};

