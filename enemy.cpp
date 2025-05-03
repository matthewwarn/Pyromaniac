// Local includes:
#include "enemy.h"

// Library includes:
#include <cstdlib>
#include <ctime>


bool Enemy::Initialise(Renderer& renderer, Texture& texture, int screenWidth, int screenheight) {
	m_screenWidth = screenWidth;
	m_screenHeight = screenheight;

	return m_sprite.Initialise(texture);
}

void Enemy::Process(float deltaTime, const Vector2& playerPos) {
	
}

void Enemy::Draw(Renderer& renderer) {
	m_sprite.SetX(static_cast<int>(m_position.x));
	m_sprite.SetY(static_cast<int>(m_position.y));
	m_sprite.Draw(renderer);
}

void Enemy::SpawnOffScreen() {
	// Randomly choose a side of the screen to spawn on
	int side = rand() % 4;

	switch (side) {
	case 0: // Top
		m_position.x = rand() % m_screenWidth;
		m_position.y = -100; 
		break;
	
	case 1: // Bottom
		m_position.x = rand() % m_screenWidth;
		m_position.y = m_screenHeight + 100;
		break;
	
	case 2: // Left
		m_position.x = -100;
		m_position.y = rand() % m_screenHeight;
		break;

	case 3: // Right
		m_position.x = m_screenWidth + 100;
		m_position.y = rand() % m_screenHeight;
		break;
	}
}

void Enemy::MoveTowardsPlayer(const Vector2& playerPos, float deltaTime) {
	Vector2 direction = playerPos - m_position;
	direction.Normalise();

	m_position += direction * m_speed * deltaTime;

}