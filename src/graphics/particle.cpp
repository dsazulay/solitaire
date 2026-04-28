#include "particle.h"

#include "../timer.h"
#include "../utils/random.h"

#include <glm/common.hpp>

auto ParticleSystem::init(ParticleConfig config) -> void
{
    m_config = config;

    m_particles.reserve(m_config.amount);
    for (unsigned int i = 0; i < m_config.amount; ++i)
    {
        m_particles.push_back(Particle());
    }

    start();
}

auto ParticleSystem::start() -> void
{
    if (m_config.spawnRate == 0)
    {
        for (unsigned int i = 0; i < m_config.amount; ++i)
        {
            respawn(m_particles[i]);
        }
    }
    else
    {
        m_spawnInterval = 1.0f / m_config.spawnRate;
        m_spawnTime = 1.0f;
    }
    m_lastUsedParticle = 0;
}

auto ParticleSystem::update() -> void
{
    m_spawnTime += Timer::deltaTime;
    if (m_config.spawnRate > 0 && m_spawnTime >= m_spawnInterval)
    {
        int index = firstUnused();
        if (index != -1)
        {
            respawn(m_particles[index]);
            m_spawnTime = 0.0f;
        }
    }
    for (unsigned int i = 0; i < m_config.amount; ++i)
    {
        Particle& p = m_particles[i];
        p.life -= Timer::deltaTime;
        if (p.life > 0.0f)
        {
            glm::vec3 velocity = glm::mix(m_config.endVelocity, p.velocity, p.life);
            p.pos += velocity * Timer::deltaTime;
            p.color.a = glm::min(p.life, 1.f);
        }
    }
}

auto ParticleSystem::particles() -> std::span<Particle>
{
    return m_particles;
}

auto ParticleSystem::firstUnused() -> int
{
    for (unsigned int i = m_lastUsedParticle; i < m_config.amount; ++i)
    {
        if (m_particles[i].life <= 0.0f)
        {
            m_lastUsedParticle = i;
            return i;
        }
    }
    for (unsigned int i = 0; i < m_lastUsedParticle; ++i)
    {
        if (m_particles[i].life <= 0.0f)
        {
            m_lastUsedParticle = i;
            return i;
        }
    }
    // overried first (oldest) particle
    //m_lastUsedParticle = 0;
    return -1;
}

auto ParticleSystem::respawn(Particle& p) -> void
{
    p.pos = glm::vec3(Random::randFloat() * 2.f - 1.f, 0.f, 0.f);

    p.velocity.x = Random::randFloat() * 15.f - 7.5f;
    p.velocity.y = Random::randFloat() * 15.f - 7.5f;


    p.color.r = Random::randFloat();
    p.color.g = Random::randFloat();
    p.color.b = Random::randFloat();
    p.color.a = 1.f;

    p.life = Random::randFloat() * 2.f + 1.f;

    m_config.endVelocity = glm::vec3{ 0.f };

}
