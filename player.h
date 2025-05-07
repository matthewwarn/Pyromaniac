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
	void DrawSprite(Renderer& renderer);
	void Movement(float deltaTime, InputSystem& inputSystem);
	

	void TakeDamage();
	bool IsAlive();

	bool CanAttack();
	void DrawHeatBar(Renderer& renderer);

	enum class Direction { Left, Right };

	Vector2& GetPosition();
	Direction GetFacingDirection();
	int GetRadius();
	int GetHealth();
	void SetInvincible();

	void SetPosition(const Vector2& pos);

	void SetZeroOverheat();
	void HandleFlamethrower(float deltaTime);
	void HandlePowerups(float deltaTime);

	// Member data:
public:
	int m_health;

	Direction m_facingDirection;

	float m_weaponHeat = 0.0f;
	float m_heatIncreaseRate = 50.0f;
	float m_heatDecreaseRate = 25.0f;
	float m_maxHeat = 250.0f;

	bool m_isOverheated = false;
	float m_overheatCooldown = 5.0f;
	float m_currentOverheatCooldown = 0.0f;

	float m_invincibleTimer = 10.0f;
	float m_zeroOverheatTimer = 15.0f;
private:
	Sprite m_sprite;
	Vector2 m_position;
	float m_speed;

	int m_screenWidth;
	int m_screenHeight;
	int m_spriteWidth;
	int m_spriteHeight;

	bool m_isAttacking;
	bool m_isInvincible;
	bool m_zeroOverheatActive;

};

#endif // PLAYER_H