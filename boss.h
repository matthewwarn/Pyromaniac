#ifndef BOSS_H
#define BOSS_H

#include "enemy.h"

// BOSS: Fat and Dangerous

class Boss : public Enemy
{
	// Member methods:
public:
	bool Initialise(Renderer& renderer, Texture& texture, int screenWidth, int screenHeight) override {

		if (!Enemy::Initialise(renderer, texture, screenWidth, screenHeight)) {
			return false;
		}

		m_originalSpeed = 70.0f;
		m_speed = 70.0f;
		m_health = 3000.0f;

		m_sprite.SetScale(1.0f);

		return true;
	}

	void Process(float deltaTime, const Vector2& playerPos) override {
		Enemy::Process(deltaTime, playerPos);
	}
};

#endif // BOSS_H