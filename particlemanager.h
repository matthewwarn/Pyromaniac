#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include "renderer.h"
#include "particle.h"

#include <vector>

class Renderer;
class Sprite;

class ParticleManager {
public:
	ParticleManager(Renderer& renderer);
	~ParticleManager();

	void Update(float dt);
	void Draw(Renderer& renderer);
	void SpawnParticles(ParticleType type, const Vector2& pos, int count);
	void Clear();

	Sprite* m_fireParticleSprite = nullptr;
	Sprite* m_powerupParticleSprite = nullptr;

private:
	std::vector<Particle> m_particles;
};

#endif // PARTICLEMANAGER_H