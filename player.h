#ifndef PLAYER_H
#define PLAYER_H

#include "vector2.h"
#include "sprite.h"

class Renderer;
class Texture;
class InputSystem;

class Player
{
	// Member methods:
public:
	Player();
	~Player();
	bool Initialise(Renderer& renderer, Texture& texture);
	void Process(float deltaTime, InputSystem& inputSystem);
	void Draw(Renderer& renderer);
	void Movement(float deltaTime, InputSystem& inputSystem);

	void TakeDamage();
	bool IsAlive();

	bool IsAttacking();

	enum class Direction { Left, Right };

	Vector2& GetPosition();
	Direction GetFacingDirection();
	int GetRadius();
	int GetHealth();

	void SetPosition(const Vector2& pos);

	// Member data:
public:
	int m_health;

	Direction m_facingDirection;


private:
	Sprite m_sprite;
	Vector2 m_position;
	float m_speed;

	int m_screenWidth;
	int m_screenHeight;
	int m_spriteWidth;
	int m_spriteHeight;

	bool m_isAttacking;
};

#endif // PLAYER_H