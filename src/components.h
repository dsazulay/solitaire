#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

constexpr const float DEFAULT_SCALE = 76.0;

struct Card
{
    int number{};
    int suit{};
};

struct Sprite
{
    // NOTE: add reference to a texture id if necessary
    glm::vec2 uv{};
};

struct Transform
{
    Transform()
    {
        model_[0][0] = DEFAULT_SCALE;
        model_[1][1] = DEFAULT_SCALE;
    }

    auto pos(glm::vec3 p) -> void
    {
        pos_ = p;
        updateModel();
    }

    auto pos() -> glm::vec3
    {
        return pos_;
    }

    auto posZ(float z) -> void
    {
        pos_.z = z;
        updateModel();
    }

    auto scale(glm::vec2 s) -> void
    {
        model_[0][0] = s.x;
        model_[1][1] = s.y;
    }

    auto model() -> glm::mat4
    {
        return model_;
    }

private:
    auto updateModel() -> void
    {
        model_[3] = glm::vec4{ pos_, 1.0 };
    }

    // NOTE: add rotation and scale if necessary
    glm::vec3 pos_{ 0.0 };
    glm::mat4 model_{ 1.0 };
};

