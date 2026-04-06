#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <random>
#include <span>

struct Particle
{
    glm::vec3 pos{};
    glm::vec3 velocity{};
    glm::vec4 color{};
    float life{};
};

struct ParticleConfig
{
    glm::vec3 startPos{};
    glm::vec3 startVelocity{};
    glm::vec4 startColor{};
    float startLife{};
    glm::vec3 endVelocity{};
    glm::vec4 endColor{};
    unsigned int amount{};
    unsigned int spawnRate{};
};

class ParticleSystem
{
public:
    auto init(std::default_random_engine* engine, ParticleConfig config) -> void;
    auto start() -> void;
    auto update() -> void;

    auto particles() -> std::span<Particle>;
private:
    auto firstUnused() -> int;
    auto respawn(Particle& p) -> void;

    ParticleConfig m_config;
    float m_spawnTime;
    float m_spawnInterval;
    std::vector<Particle> m_particles;
    unsigned int m_lastUsedParticle;

    std::default_random_engine* randomEngine;
};

