// Local includes:
#include "enemy.h"

// Library includes:
#include <cstdlib>
#include <ctime>
#include <ostream>
#include <iostream>
#include <string>

Enemy::Enemy()
	: m_position(0.0f, 0.0f)
	, m_speed(0.0f)
	, m_originalSpeed(0.0f)
	, m_health(100)
	, m_takingDamage(false)
	, m_screenWidth(0)
	, m_screenHeight(0) 
{
}

Enemy::~Enemy() {
	delete m_sprite;
}

bool Enemy::Initialise(Renderer& renderer, const char* spritePath, int screenWidth, int screenHeight) {
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	
	m_sprite = renderer.CreateSprite(spritePath);

	return (m_sprite != nullptr);
}

void Enemy::Process(float deltaTime, const Vector2& playerPos) {
	if (m_takingDamage) {
		// Damage Indicator
		m_sprite->SetRedTint(1.0f);
		m_sprite->SetGreenTint(0.0f);
		m_sprite->SetBlueTint(0.0f);

		// Slow Enemy
		m_speed = m_originalSpeed * 0.5f;
	} 
	else {
		m_sprite->SetRedTint(1.0f);
		m_sprite->SetGreenTint(1.0f);
		m_sprite->SetBlueTint(1.0f);

		// Reset speed
		m_speed = m_originalSpeed;
	}

	m_animatedSprite->Process(deltaTime);
	MoveTowardsPlayer(playerPos, deltaTime);
}

void Enemy::Draw(Renderer& renderer) {
	m_animatedSprite->SetX(static_cast<int>(m_position.x));
	m_animatedSprite->SetY(static_cast<int>(m_position.y));

	m_animatedSprite->Draw(renderer);
}

Vector2& Enemy::GetPosition()
{
	return m_position;
}

void Enemy::SpawnOffScreen() {
	// Randomly choose a side of the screen to spawn on
	int side = rand() % 4;

	switch (side) {
	case 0: // Top
		m_position.x = static_cast<float>(rand() % m_screenWidth);
		m_position.y = -200.0f; 
		break;
	
	case 1: // Bottom
		m_position.x = static_cast<float>(rand() % m_screenWidth);
		m_position.y = m_screenHeight + 200.0f;
		break;
	
	case 2: // Left
		m_position.x = -200.0f;
		m_position.y = static_cast<float>(rand() % m_screenHeight);
		break;

	case 3: // Right
		m_position.x = m_screenWidth + 200.0f;
		m_position.y = static_cast<float>(rand() % m_screenHeight);;
		break;
	}
}

void Enemy::MoveTowardsPlayer(const Vector2& playerPos, float deltaTime) {
	Vector2 direction = playerPos - m_position;
	direction.Normalise();

	m_position += direction * m_speed * deltaTime;
}

int Enemy::GetRadius() {
	return m_sprite->GetWidth() / 2;
}

float Enemy::GetSpeed()
{
	return m_speed;
}

float Enemy::GetHealth() {
	return m_health;
}

int Enemy::GetPointsValue() {
	return m_points;
}

void Enemy::SetSpeed(float newSpeed)
{
	m_speed = newSpeed;
}

void Enemy::SetTakingDamage(bool takingDamage)
{
	m_takingDamage = takingDamage;
}

void Enemy::TakeDamage(float amount)
{
	m_health -= amount;
}

bool Enemy::IsAlive() {
	return m_health > 0;
}