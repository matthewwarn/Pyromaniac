#ifndef ENEMYTYPE3_H
#define ENEMYTYPE3_H

#include "enemy.h"

// ENEMY 3: Slow and Tanky

class EnemyType3 : public Enemy
{
	// Member methods:
public:
	bool Initialise(Renderer& renderer, const char* spritePath, int screenWidth, int screenHeight) override {
		if (!Enemy::Initialise(renderer, spritePath, screenWidth, screenHeight)) {
			return false;
		}

		m_originalSpeed = 40.0f;
		m_speed = 40.0f;
		m_health = 250.0f;

		m_sprite->SetScale(0.3f);

		return true;
	}

	void Process(float deltaTime, const Vector2& playerPos) override
	{
		Enemy::Process(deltaTime, playerPos);
	}
};

#endif // ENEMYTYPE3_H