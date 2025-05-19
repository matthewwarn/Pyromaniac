#ifndef ENEMY_H
#define ENEMY_H

#include "renderer.h"
#include "vector2.h"
#include "sprite.h"
#include "texture.h"
#include "animatedsprite.h"

class Renderer;
class Texture;
class Sprite;

class Enemy {
	// Member methods:
public:
	Enemy();
	virtual ~Enemy();

	virtual bool Initialise(Renderer& renderer, const char* spritePath, int screenWidth, int screenHeight);
	virtual void Process(float deltaTime, const Vector2& playerPos);
	virtual void Draw(Renderer& renderer);

	Vector2& GetPosition();

	void SpawnOffScreen();

	void TakeDamage(float amount);
	bool IsAlive();

	virtual int GetRadius();
	float GetSpeed();
	float GetHealth();
	int GetPointsValue();
	
	void SetSpeed(float newSpeed);
	void SetTakingDamage(bool takingDamage);

protected:
	void MoveTowardsPlayer(const Vector2& playerPos, float deltaTime);

private:

	// Member data:
public:

protected:
	Vector2 m_position;
	float m_speed;
	float m_originalSpeed;
	float m_health;
	Sprite* m_sprite = nullptr;

	bool m_takingDamage;

	int m_screenWidth;
	int m_screenHeight;

	int m_points = 0;

	AnimatedSprite* m_animatedSprite = nullptr;
};

#endif // ENEMY_H