#include "player.h"
#include "inputsystem.h"
#include "renderer.h"
#include <iostream>
#include <glew.h>

Player::Player()
	: m_position(0.0f, 0.0f)
	, m_speed(200.0f)
{
}

Player::~Player()
{
}

bool Player::Initialise(Renderer& renderer, Texture& texture)
{
	m_screenWidth = renderer.GetWidth();
	m_screenHeight = renderer.GetHeight();

	m_spriteWidth = m_sprite.GetWidth();
	m_spriteHeight = m_sprite.GetHeight();
	m_sprite.SetScale(0.3f);
	m_sprite.SetGreenTint(1.0f);
	m_sprite.SetRedTint(0.0f);
	m_sprite.SetBlueTint(0.0f);

	m_health = 1;

	return m_sprite.Initialise(texture);
}

void Player::Process(float deltaTime, InputSystem& inputSystem) {
	// Handle movement
	Movement(deltaTime, inputSystem);

	// Handle weapon heat
	HandleFlamethrower(deltaTime);

	// Update position
	m_sprite.SetX(static_cast<int>(m_position.x));
	m_sprite.SetY(static_cast<int>(m_position.y));

}

void Player::Movement(float deltaTime, InputSystem& inputSystem) {
	Vector2 direction(0.0f, 0.0f);

	// Storing control button states
	ButtonState wState = inputSystem.GetKeyState(SDL_SCANCODE_W);
	ButtonState sState = inputSystem.GetKeyState(SDL_SCANCODE_S);
	ButtonState aState = inputSystem.GetKeyState(SDL_SCANCODE_A);
	ButtonState dState = inputSystem.GetKeyState(SDL_SCANCODE_D);
	ButtonState kState = inputSystem.GetKeyState(SDL_SCANCODE_K);

	if (wState == BS_HELD || wState == BS_PRESSED) {
		direction.y -= 1.0f;
	}
	if (sState == BS_HELD || sState == BS_PRESSED) {
		direction.y += 1.0f;
	}
	if (aState == BS_HELD || aState == BS_PRESSED) {
		direction.x -= 1.0f;
		m_facingDirection = Direction::Left;
	}
	if (dState == BS_HELD || dState == BS_PRESSED) {
		direction.x += 1.0f;
		m_facingDirection = Direction::Right;
	}
	if (kState == BS_HELD || kState == BS_PRESSED) {
		m_isAttacking = true;
	}
	else if (kState == BS_RELEASED) {
		m_isAttacking = false;
	}

	// If player has pressed button to move, update player position
	if (direction.LengthSquared() > 0.0f)
	{
		direction.Normalise();
		m_position += direction * m_speed * deltaTime;
	}

	// Clamp position to screen bounds

	if (m_position.x < 0) m_position.x = 0;
	if (m_position.y < 0) m_position.y = 0;
	if (m_position.x > m_screenWidth - m_spriteWidth) m_position.x = m_screenWidth - m_spriteWidth;
	if (m_position.y > m_screenHeight - m_spriteHeight) m_position.y = m_screenHeight - m_spriteHeight;
}

void Player::DrawSprite(Renderer& renderer)
{
	m_sprite.Draw(renderer);
}

Vector2& Player::GetPosition()
{
	return m_position;
}

void Player::SetPosition(const Vector2& pos) {
	m_position = pos;
}

int Player::GetHealth()
{
	return m_health;
}

void Player::TakeDamage()
{
	m_health--;
}

bool Player::IsAlive()
{
	if (m_health > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Player::GetRadius()
{
	return m_spriteWidth / 2;
}

bool Player::CanAttack()
{
	if (m_isAttacking == true && m_isOverheated == false) {
		return true;
	}
	else {
		return false;
	}
}

Player::Direction Player::GetFacingDirection() {
	return m_facingDirection;
}

void Player::HandleFlamethrower(float deltaTime) {
	if (m_isOverheated) {
		m_currentOverheatCooldown -= deltaTime;
		if (m_currentOverheatCooldown <= 0.0f) {
			m_isOverheated = false;
			m_weaponHeat = 0.0f;
		}
	}
	else {
		if (m_isAttacking) {
			m_weaponHeat += m_heatIncreaseRate * deltaTime;

			if (m_weaponHeat >= m_maxHeat) {
				m_isOverheated = true;
				m_currentOverheatCooldown = m_overheatCooldown;
			}
		}
		else {
			m_weaponHeat -= m_heatDecreaseRate * deltaTime;
			if (m_weaponHeat < 0.0f) {
				m_weaponHeat = 0.0f;
			}
		}
	}
}

void Player::DrawHeatBar(Renderer& renderer) {
	int barWidth = 40;
	int barHeight = 200;
	int barX = m_screenWidth - 40;
	int barY = 120;

	float heatPercentage = m_weaponHeat / m_maxHeat;
	int filledHeight = static_cast<int>(barHeight * heatPercentage);
	int bottomY = barHeight;

	renderer.DrawRect(barX, barY, barWidth, barHeight, 0.0f, 0.0f, 0.0f, 1.0f); // Draw empty bar

	if (m_isOverheated) {
		renderer.DrawRect(barX, barY, barWidth, filledHeight, 1.0f, 0.0f, 0.0, 1.0f); // Draw red for overheated
	}
	else {
		renderer.DrawRect(barX, barY, barWidth, filledHeight, 1.0f, 0.5f, 0.1f, 1.0f); // Draw filled portion
	}
}