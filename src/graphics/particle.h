#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <span>

struct Particle
{
    glm::vec3 pos{};
    glm::vec3 velocity{};
    glm::vec4 color{};
    float life{};
};

class ParticleSystem
{
public:
    auto init(unsigned int amount) -> void;
    auto update(float dt, unsigned int newParticles, float velocity) -> void;

    auto particles() -> std::span<Particle>;

private:
    auto firstUnused() -> int;
    auto respawn(Particle& p, float velocity) -> void;

    std::vector<Particle> m_particles;
    unsigned int m_amount;
    unsigned int m_lastUsedParticle;
};
