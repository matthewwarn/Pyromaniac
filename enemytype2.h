#ifndef ENEMYTYPE2_H
#define ENEMYTYPE2_H

#include "enemy.h"

// ENEMY 2: Fast and Weak

class EnemyType2 : public Enemy
{
	// Member methods:
public:
	bool Initialise(Renderer& renderer, const char* spritePath, int screenWidth, int screenHeight) override {

		if (!Enemy::Initialise(renderer, spritePath, screenWidth, screenHeight)) {
			return false;
		}

		m_originalSpeed = 125.0f;
		m_speed = 125.0f;
		m_health = 20.0f;

		m_sprite->SetScale(0.15f);

		return true;
	}

	void Process(float deltaTime, const Vector2& playerPos) override {
		Enemy::Process(deltaTime, playerPos);
	}
};

#endif // ENEMYTYPE2_H