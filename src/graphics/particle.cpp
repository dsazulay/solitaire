#include "particle.h"

auto ParticleSystem::init(unsigned int amount) -> void
{
    m_amount = amount;
    m_particles.reserve(amount);
    for (unsigned int i = 0; i < amount; ++i)
    {
        m_particles.push_back(Particle());
    }
    m_lastUsedParticle = 0;
}

auto ParticleSystem::update(float dt, unsigned int newParticles, float velocity) -> void
{
    for (unsigned int i = 0; i < newParticles; ++i)
    {
        int index = firstUnused();
        if (index != -1)
        {
            respawn(m_particles[index], velocity);
        }
    }
    for (unsigned int i = 0; i < m_amount; ++i)
    {
        Particle& p = m_particles[i];
        p.life -= dt;
        if (p.life > 0.0f)
        {
            p.pos -= p.velocity * dt;
            p.color.a -= dt * 2.5f;
        }
    }
}

auto ParticleSystem::particles() -> std::span<Particle>
{
    return m_particles;
}

auto ParticleSystem::firstUnused() -> int
{
    for (unsigned int i = m_lastUsedParticle; i < m_amount; ++i)
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
    // overried first (oldest) particle;
    //m_lastUsedParticle = 0;
    return -1;
}

auto ParticleSystem::respawn(Particle& p, float velocity) -> void
{
    float random = ((rand() % 100) - 50) / 50.0f;
    float color = 0.5f + ((rand() % 100) / 100.0f);
    p.pos = glm::vec3(random, 0.0, 0.0);
    p.velocity = glm::vec3((((rand() % 10) - 5) / 10.f), velocity * ((rand() % 10) / 10.f), 0.0);
    p.color = glm::vec4(color, color, color, 1.0f);
    p.life = 2.0f + 2.0f * ((rand() % 10) / 10.0f);

}
