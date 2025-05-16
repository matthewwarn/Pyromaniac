#include "player.h"
#include "inputsystem.h"
#include "renderer.h"
#include <iostream>
#include <glew.h>
#include "xboxcontroller.h"

Player::Player()
	: m_position(0.0f, 0.0f)
	, m_speed(200.0f)
{
}

Player::~Player()
{
}

bool Player::Initialise(Renderer& renderer)
{
	m_screenWidth = renderer.GetWidth();
	m_screenHeight = renderer.GetHeight();


	// PLAYER SPRITE
	m_sprite = renderer.CreateAnimatedSprite("../assets/player.png");

	if (!m_sprite) {
		std::cerr << "Failed to create animated sprite for player!" << std::endl;
		return false;
	}
	m_sprite->SetupFrames(158, 160);
	m_sprite->SetFrameDuration(0.3f);
	m_sprite->SetLooping(true);
	m_sprite->Animate();
	m_sprite->SetScale(0.8f);

	m_spriteWidth = m_sprite->GetFrameWidth();
	m_spriteHeight = m_sprite->GetFrameHeight();


	// FLAME SPRITE
	m_attackSprite = renderer.CreateAnimatedSprite("../assets/flame.png");
	
	if (!m_attackSprite) {
		std::cerr << "Failed to create animated sprite for flames!" << std::endl;
		return false;
	}
	m_attackSprite->SetupFrames(1260, 310);
	m_attackSprite->SetFrameDuration(0.1f);
	m_attackSprite->SetLooping(true);
	m_attackSprite->Animate();
	m_attackSprite->SetAlpha(0.8f);
	m_attackSprite->SetScale(0.25f);

	m_health = 1;
	m_isInvincible = false;
	m_zeroOverheatActive = false;

	return true;
}

void Player::Process(float deltaTime, InputSystem& inputSystem) {

	if (!IsAlive()) {
		return;
	}

	// Handle movement
	Movement(deltaTime, inputSystem);

	// Handle weapon heat
	HandleFlamethrower(deltaTime);

	// Handle powerups e.g. Invincibility, No Overheat
	HandlePowerups(deltaTime);

	// Update position
	m_sprite->SetX(static_cast<int>(m_position.x));
	m_sprite->SetY(static_cast<int>(m_position.y));

	m_sprite->Process(deltaTime);
	m_attackSprite->Process(deltaTime);
}

void Player::Movement(float deltaTime, InputSystem& inputSystem) {
	Vector2 direction(0.0f, 0.0f);

	// Storing control button states
	ButtonState wState = inputSystem.GetKeyState(SDL_SCANCODE_W);
	ButtonState sState = inputSystem.GetKeyState(SDL_SCANCODE_S);
	ButtonState aState = inputSystem.GetKeyState(SDL_SCANCODE_A);
	ButtonState dState = inputSystem.GetKeyState(SDL_SCANCODE_D);
	ButtonState kState = inputSystem.GetKeyState(SDL_SCANCODE_K);

	// Keyboard input
	if (wState == BS_HELD || wState == BS_PRESSED) {
		direction.y -= 1.0f;
	}
	if (sState == BS_HELD || sState == BS_PRESSED) {
		direction.y += 1.0f;
	}
	if (aState == BS_HELD || aState == BS_PRESSED) {
		direction.x -= 1.0f;
		m_facingDirection = Direction::Left;
		m_sprite->SetFlipX(true);
	}
	if (dState == BS_HELD || dState == BS_PRESSED) {
		direction.x += 1.0f;
		m_facingDirection = Direction::Right;
		m_sprite->SetFlipX(false);
	}
	if (kState == BS_HELD || kState == BS_PRESSED) {
		m_isAttacking = true;
	}
	else if (kState == BS_RELEASED) {
		m_isAttacking = false;
	}

	// Gamepad input
	XboxController* controller = inputSystem.GetController(0);

	if (controller && controller->IsConnected()) {
		Vector2 leftStick = controller->GetLeftStick();

		if (leftStick.LengthSquared() > 0.1f) {
			direction += leftStick;

			if (leftStick.x > 0.1f){
				m_facingDirection = Direction::Right;
				m_sprite->SetFlipX(false);
			}
			else if (leftStick.x < -0.1f) {
				m_facingDirection = Direction::Left;
				m_sprite->SetFlipX(true);
			}
		}

		if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_X) == BS_PRESSED) {
			m_isAttacking = true;
		}
		else if (controller->GetButtonState(SDL_CONTROLLER_BUTTON_X) == BS_RELEASED) {
			m_isAttacking = false;
		}
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

	if (m_position.x > m_screenWidth - (m_sprite->GetWidth() - (m_sprite->GetFrameWidth() * 0.8))) {
		m_position.x = m_screenWidth - (m_sprite->GetWidth() - (m_sprite->GetFrameWidth() * 0.8));
	}
	if (m_position.y > m_screenHeight - (m_sprite->GetHeight() - (m_sprite->GetFrameHeight() * 0.8))) {
		m_position.y = m_screenHeight - (m_sprite->GetHeight() - (m_sprite->GetFrameHeight()* 0.8));
	}
}

void Player::DrawSprite(Renderer& renderer)
{
	m_sprite->Draw(renderer);
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
	if (m_isInvincible) {
		return; // Don't take damage if invincible
	}
	else {
		m_health--;
	}
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
	return (m_sprite->GetFrameWidth() / 2) * m_sprite->GetScale() * 0.2;
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

void Player::SetZeroOverheat()
{
	m_zeroOverheatActive = true;
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
	int barWidth = 50;
	int barHeight = m_screenHeight * 0.8;
	int barX = m_screenWidth - barWidth;
	int barY = m_screenHeight / 2;

	float heatPercentage = m_weaponHeat / m_maxHeat;
	int filledHeight = static_cast<int>(barHeight * heatPercentage);
	int bottomY = barHeight;

	renderer.DrawRect(barX, barY, barWidth, barHeight, 0.0f, 0.0f, 0.0f, 0.7f); // Draw empty bar

	if (m_zeroOverheatActive) {
		renderer.DrawRect(barX, barY, barWidth, barHeight, 0.6f, 0.72f, 0.925f, 0.7f); // Draw blue for zero overheat
	}
	else if (m_isOverheated) {
		renderer.DrawRect(barX, barY, barWidth, barHeight, 1.0f, 0.0f, 0.0, 0.7f); // Draw red for overheated
	}
	else {
		renderer.DrawRect(barX, barY, barWidth, filledHeight, 1.0f, 0.5f, 0.1f, 0.7f); // Draw filled portion
	}
	
}

int Player::GetScore()
{
	return m_score;
}

void Player::EditScore(int points) 
{
	m_score += points;
}

void Player::SetInvincible() {
	m_isInvincible = true;
}

void Player::SetHealth(int health) {
	m_health = health;
}

void Player::HandlePowerups(float deltaTime) {
	// Invincibility
	if (m_isInvincible) {
		m_invincibleTimer -= deltaTime; // Decrease timer

		// Visual Indicator
		m_sprite->SetRedTint(1.0f);
		m_sprite->SetGreenTint(0.0f);
		m_sprite->SetBlueTint(0.0f);

		if (m_invincibleTimer <= 0.0f) {
			m_isInvincible = false;
			m_invincibleTimer = 10.0f; // Reset timer
		}
	}
	else {
		// Set sprite back to normal
		m_sprite->SetRedTint(1.0f);
		m_sprite->SetGreenTint(1.0f);
		m_sprite->SetBlueTint(1.0f);
	}

	// Zero Overheat
	if (m_zeroOverheatActive) {
		m_zeroOverheatTimer -= deltaTime;
		m_isOverheated = false;
		m_weaponHeat = 0.0f; // Keep heat at 0

		if (m_zeroOverheatTimer <= 0.0f) {
			m_zeroOverheatActive = false;
			m_zeroOverheatTimer = 15.0f; // Reset timer
		}
	}
}

void Player::ResetPlayer()
{
	m_health = 1;
	m_score = 0;
	m_isInvincible = false;
	m_zeroOverheatActive = false;
	m_weaponHeat = 0.0f;
	m_isOverheated = false;
	m_currentOverheatCooldown = 0.0f;
	m_isAttacking = false;

	// Reset sprite colour
	m_sprite->SetRedTint(1.0f);
	m_sprite->SetGreenTint(1.0f);
	m_sprite->SetBlueTint(1.0f);
}
