#ifndef PARTICLE_H
#define PARTICLE_H

#include <SDL.h>
#include "vector2.h"
#include "sprite.h"

class Renderer;
class ParticleManager;

enum class ParticleType
{
	Fire,
	Powerup
};

class Particle 
{
public:
	Particle(Vector2 position, Vector2 velocity, float lifetime, SDL_Color color, float size, ParticleType type);

	void Update(float deltaTime);
	void Draw(Renderer& renderer, const ParticleManager& pm);

	bool IsAlive() const;


protected:

private:
	Vector2 m_position;
	Vector2 m_velocity;
	float m_lifetime;
	float m_maxLifetime;
	SDL_Color m_color;
	float m_size;
	ParticleType m_type;
	Sprite* m_particleSprite = nullptr;
};

#endif // PARTICLE_H