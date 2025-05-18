#include "particle.h"
#include "particlemanager.h"
#include "renderer.h"

#include <cstdlib>

Particle::Particle(Vector2 position, Vector2 velocity, float lifetime, SDL_Color color, float size, ParticleType type)
	: m_position(position)
	, m_velocity (velocity)
	, m_lifetime(lifetime)
	, m_maxLifetime(lifetime)
	, m_color(color)
	, m_size(size)
	, m_type(type)
{
}

void Particle::Update(float deltaTime)
{
	m_lifetime -= deltaTime;
	m_position = m_position + m_velocity * deltaTime;

	// Fade away
	float alpha = m_lifetime / m_maxLifetime;
	m_color.a = static_cast<Uint8>(alpha * 255);

	// Type-specific behavior
	switch (m_type)
	{
	case ParticleType::Fire:
		break;
	case ParticleType::Powerup:
		m_position.x += sinf(m_lifetime * 10.0f) * 5.0f * deltaTime; // Wiggle
		break;
	default:
		break;
	}
}

void Particle::Draw(Renderer& renderer, const ParticleManager& pm)
{
	// Determine Sprite
	switch (m_type) 
	{
	case ParticleType::Fire:
		m_particleSprite = pm.m_fireParticleSprite;
		break;
	case ParticleType::Powerup:
		m_particleSprite = pm.m_powerupParticleSprite;
		break;
	default:
		break;
	}

	m_particleSprite->SetX(m_position.x);
	m_particleSprite->SetY(m_position.y);
	m_particleSprite->SetScale(m_size);

	m_particleSprite->SetRedTint(m_color.r / 255.0f);
	m_particleSprite->SetGreenTint(m_color.g / 255.0f);
	m_particleSprite->SetBlueTint(m_color.b / 255.0f);
	m_particleSprite->SetAlpha(m_color.a / 255.0f);

	m_particleSprite->Draw(renderer);
}

bool Particle::IsAlive() const
{
	return m_lifetime > 0.0f;
}