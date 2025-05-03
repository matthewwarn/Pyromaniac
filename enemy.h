#ifndef ENEMY_H
#define ENEMY_H

#include "renderer.h"
#include "vector2.h"
#include "sprite.h"
#include "texture.h"

class Renderer;
class Texture;
class Sprite;

class Enemy {
	// Member methods:
public:
	virtual ~Enemy() {}

	virtual bool Initialise(Renderer& renderer, Texture& texture, int screenWidth, int screenHeight);
	virtual void Process(float deltaTime, const Vector2& playerPos);
	virtual void Draw(Renderer& renderer);

	Vector2& GetPosition();

	void SpawnOffScreen();

protected:
	void MoveTowardsPlayer(const Vector2& playerPos, float deltaTime);

private:

	// Member data:
public:

protected:
	Vector2 m_position;
	float m_speed;
	int m_health;
	Sprite m_sprite;

	int m_screenWidth;
	int m_screenHeight;
};

#endif // ENEMY_H