#include "particle.h"
#include "../timer.h"

auto ParticleSystem::init(std::default_random_engine* engine, ParticleConfig config) -> void
{
    randomEngine = engine;
    m_config = config;

    m_particles.reserve(m_config.amount);
    for (unsigned int i = 0; i < m_config.amount; ++i)
    {
        m_particles.push_back(Particle());
    }

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
            p.pos -= p.velocity * Timer::deltaTime;
            //p.color.a -= dt * 2.5f;
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
    std::uniform_real_distribution<float> uniformDist(-1.0f, 1.0f);

    p.pos = glm::vec3(uniformDist(*randomEngine), 0.0, 0.0);

    p.velocity.x = uniformDist(*randomEngine) * 10.0f;
    p.velocity.y = uniformDist(*randomEngine) * 10.0f;

    p.color.r = uniformDist(*randomEngine) * 0.5f + 0.5f;
    p.color.g = uniformDist(*randomEngine) * 0.5f + 0.5f;
    p.color.b = uniformDist(*randomEngine) * 0.5f + 0.5f;
    p.color.a = 1.0f;
    
    p.life = uniformDist(*randomEngine) + 3.0f;

}
