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
	, m_screenHeight(0) {
}

Enemy::~Enemy() {
	
}

bool Enemy::Initialise(Renderer& renderer, Texture& texture, int screenWidth, int screenheight) {
	m_screenWidth = screenWidth;
	m_screenHeight = screenheight;

	return m_sprite.Initialise(texture);
}

void Enemy::Process(float deltaTime, const Vector2& playerPos) {
	if (m_takingDamage) {
		// Damage Indicator
		m_sprite.SetRedTint(1.0f);
		m_sprite.SetGreenTint(0.0f);
		m_sprite.SetBlueTint(0.0f);

		// Slow Enemy
		m_speed = m_originalSpeed * 0.5f;
	} 
	else {
		m_sprite.SetRedTint(1.0f);
		m_sprite.SetGreenTint(1.0f);
		m_sprite.SetBlueTint(1.0f);

		// Reset speed
		m_speed = m_originalSpeed;
	}

	MoveTowardsPlayer(playerPos, deltaTime);
}

void Enemy::Draw(Renderer& renderer) {
	m_sprite.SetX(static_cast<int>(m_position.x));
	m_sprite.SetY(static_cast<int>(m_position.y));
	m_sprite.Draw(renderer);
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
		m_position.x = rand() % m_screenWidth;
		m_position.y = -200; 
		break;
	
	case 1: // Bottom
		m_position.x = rand() % m_screenWidth;
		m_position.y = m_screenHeight + 200;
		break;
	
	case 2: // Left
		m_position.x = -200;
		m_position.y = rand() % m_screenHeight;
		break;

	case 3: // Right
		m_position.x = m_screenWidth + 200;
		m_position.y = rand() % m_screenHeight;
		break;
	}
}

void Enemy::MoveTowardsPlayer(const Vector2& playerPos, float deltaTime) {
	Vector2 direction = playerPos - m_position;
	direction.Normalise();

	m_position += direction * m_speed * deltaTime;
}

int Enemy::GetRadius() {
	return m_sprite.GetWidth() / 2;
}

float Enemy::GetSpeed()
{
	return m_speed;
}

float Enemy::GetHealth() {
	return m_health;
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