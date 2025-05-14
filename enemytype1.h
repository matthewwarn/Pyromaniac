#ifndef ENEMYTYPE1_H
#define ENEMYTYPE1_H

#include "enemy.h"

// ENEMY 1: Sturdy and Steady

class EnemyType1 : public Enemy
{
	// Member methods:
public:
	bool Initialise(Renderer& renderer, const char* spritePath, int screenWidth, int screenHeight) override {
		
		if (!Enemy::Initialise(renderer, spritePath, screenWidth, screenHeight)) {
			return false;
		}
		
		m_originalSpeed = 60.0f;
		m_speed = 60.0f;
		m_health = 80.0f;

		m_sprite->SetScale(0.2f);

		return true;
	}

	void Process(float deltaTime, const Vector2& playerPos) override {
		Enemy::Process(deltaTime, playerPos);
	}
};

#endif // ENEMYTYPE1_H