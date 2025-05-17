#ifndef PARTICLEMANAGER_H
#define PARTICLEMANAGER_H

#include "renderer.h"
#include "particle.h"

#include <vector>


class ParticleManager {
public:
	void Update(float dt);
	void Draw(Renderer& renderer);
	void SpawnParticles(ParticleType type, const Vector2& pos, int count);

private:
	std::vector<Particle> m_particles;
};

#endif // PARTICLEMANAGER_H