#ifndef ENEMYTYPE1_H
#define ENEMYTYPE1_H

#include "enemy.h"

class EnemyType1 : public Enemy
{
	// Member methods:
public:
	bool Initialise(Renderer& renderer, Texture& texture, int screenWidth, int screenHeight) override {
		m_originalSpeed = 35.0f;
		m_speed = 35.0f;
		m_health = 300;

		m_sprite.SetScale(0.2f);

		return Enemy::Initialise(renderer, texture, screenWidth, screenHeight);
	}

	void Process(float deltaTime, const Vector2& playerPos) override {
		MoveTowardsPlayer(playerPos, deltaTime);
		Enemy::Process(deltaTime, playerPos);
	}
};

#endif // ENEMYTYPE1_H