#ifndef PLAYER_H
#define PLAYER_H

#include "vector2.h"
#include "sprite.h"
#include "animatedsprite.h"

class Renderer;
class Texture;
class InputSystem;

class Player
{
	// Member methods:
public:
	Player();
	~Player();
	bool Initialise(Renderer& renderer);
	void Process(float deltaTime, InputSystem& inputSystem, Renderer& renderer);
	void DrawSprite(Renderer& renderer);
	void Movement(float deltaTime, InputSystem& inputSystem);
	
	void TakeDamage();
	bool IsAlive();

	bool CanAttack();
	void DrawHeatBar(Renderer& renderer);

	int GetScore();
	void EditScore(int points);

	enum class Direction { Left, Right };

	Vector2& GetPosition();
	Direction GetFacingDirection();
	int GetRadius();
	int GetHealth();
	void SetHealth(int health);
	void SetInvincible();

	void SetPosition(const Vector2& pos);

	void SetZeroOverheat();
	void HandleFlamethrower(float deltaTime);
	void HandlePowerups(float deltaTime);

	void ResetPlayer();

	// Member data:
public:
	int m_health;
	int m_score = 0;

	Direction m_facingDirection = Direction::Right;

	float m_weaponHeat = 0.0f;
	float m_heatIncreaseRate = 50.0f;
	float m_heatDecreaseRate = 35.0f;
	float m_maxHeat = 350.0f; // 7 Seconds of Fire
	bool m_isAttacking;
	bool m_attackAnimPlaying = false;
	AnimatedSprite* GetFlameSprite() { return m_attackSprite; }

	bool m_isOverheated = false;
	float m_overheatCooldown = 4.5f;
	float m_currentOverheatCooldown = 0.0f;

	float m_invincibleTimer = 10.0f;
	float m_zeroOverheatTimer = 15.0f;

	AnimatedSprite* m_sprite;

	int m_screenWidth;
	int m_screenHeight;

private:
	AnimatedSprite* m_attackSprite;
	Vector2 m_position;
	float m_speed;

	int m_spriteWidth;
	int m_spriteHeight;

	bool m_isInvincible;
	bool m_zeroOverheatActive;

};

#endif // PLAYER_H