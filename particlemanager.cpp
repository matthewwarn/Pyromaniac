#include "particlemanager.h"
#include <cstdlib>

static float RandFloat(float min, float max) 
{
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void ParticleManager::Update(float deltaTime)
{
	for (auto it = m_particles.begin(); it != m_particles.end();)
	{
		it->Update(deltaTime);
		if (!it->IsAlive())
		{
			it = m_particles.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ParticleManager::Draw(Renderer& renderer)
{
	for (auto& particle : m_particles)
	{
		particle.Draw(renderer);
	}
}

void ParticleManager::SpawnParticles(ParticleType type, const Vector2& position, int count)
{
	for (int i = 0; i < count; ++i)
	{
		// Default values
		Vector2 velocity(RandFloat(-50.0f, 50.0f), RandFloat(-50.0f, 50.0f));
		float lifetime = RandFloat(0.5f, 1.5f);
		float size = RandFloat(0.1f, 0.3f);
		SDL_Color color = { 255, 255, 255, 255 };

		switch (type)
		{
		case ParticleType::Fire:
			velocity = Vector2(RandFloat(-250.0f, 250.0f), RandFloat(-100.0f, 100.0f));
			size *= 0.7;
			lifetime = RandFloat(1.5, 3.0);
			break;
		case ParticleType::Powerup:
			lifetime = RandFloat(1.0f, 2.5f);
			color = { 180, 0, 255, 255 }; // Purple
			size *= 0.15;
			break;
		default:
			break;
		}

		m_particles.emplace_back(position, velocity, lifetime, color, size, type);
	}
}