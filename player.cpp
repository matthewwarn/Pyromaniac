#include "player.h"
#include "inputsystem.h"
#include "renderer.h"
#include <iostream>

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

void Player::Draw(Renderer& renderer)
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

bool Player::IsAttacking()
{
	return m_isAttacking;
}

Player::Direction Player::GetFacingDirection() {
	return m_facingDirection;
}