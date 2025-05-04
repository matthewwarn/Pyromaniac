// This include:
#include "scenemain.h"

// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "imgui/imgui.h"
#include "inputsystem.h"
#include "game.h"
#include "logmanager.h"
#include "player.h"
#include "enemytype1.h"
#include "enemy.h"


// Library includes:
#include <cassert>
#include "fmod.hpp"
#include <ctime>
#include <iostream>

SceneMain::SceneMain()
	: m_enemySpawnTimer(0.0f)
	, m_enemySpawnInterval(0.2f)
	, m_gameTimer(0.0f)
{
}

SceneMain::~SceneMain() 
{
}

bool
SceneMain::Initialise(Renderer& renderer)
{
	m_pRenderer = &renderer;

	srand(static_cast<unsigned int>(time(0)));

	// Load Player Texture
	static Texture* playerTexture = new Texture();
	playerTexture->Initialise("../assets/ball.png"); // Replace later
	m_player.Initialise(renderer, *playerTexture);

	// Spawn in center
	int screenWidth = renderer.GetWidth();
	int screenHeight = renderer.GetHeight();
	Vector2 startPos(screenWidth / 2, screenHeight / 2);
	m_player.SetPosition(startPos);

	return true;
}

void
SceneMain::Process(float deltaTime, InputSystem& inputSystem) 
{
	m_player.Process(deltaTime, inputSystem);

	m_gameTimer += deltaTime;

	// Spawn Enemies
	m_enemySpawnTimer += deltaTime;
	if (m_enemySpawnTimer >= m_enemySpawnInterval) {
		SpawnEnemy();
		m_enemySpawnTimer = 0.0f;
	}

	processEnemies(deltaTime);

	handleAttackCollisions(deltaTime);

	// Check if player is alive
	if (!m_player.IsAlive()) {
		LogManager::GetInstance().Log("Player is dead!");
		// Lose screen here
	}
}

void
SceneMain::Draw(Renderer& renderer)
{
	m_player.Draw(renderer);
	
	for (auto& enemy : m_enemies) {
		enemy->Draw(renderer);
	}

	// Display Attack Hitbox
	if (m_showHitbox && m_player.IsAttacking()) {
		Vector2 playerPos = m_player.GetPosition();
		bool facingRight = (m_player.GetFacingDirection() == Player::Direction::Right);

		const float attackRange = 350.0f;
		const float attackWidth = 75.0f;

		// Flip attack if facing left
		float offsetX = 0.0f;
		if (facingRight) {
			offsetX = m_player.GetRadius();
		}
		else {
			offsetX = -(attackRange + m_player.GetRadius());
		}

		Vector2 attackPos(playerPos.x + offsetX, playerPos.y - (attackWidth / 2));

		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		ImVec2 topLeft(attackPos.x, attackPos.y);
		ImVec2 bottomRight(attackPos.x + attackRange, attackPos.y + attackWidth);

		drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(255, 0, 0, 180));
	}
}

void SceneMain::DebugDraw
()
{
	ImGui::Text("Scene: Main");

	// Display Timer
	int minutes = static_cast<int>(m_gameTimer) / 60;
	int seconds = static_cast<int>(m_gameTimer) % 60;
	ImGui::Text("Time: %02d:%02d", minutes, seconds);

	//Toggle Flamethrower Hitbox
	ImGui::Checkbox("Show Hitbox", &m_showHitbox);
}

void SceneMain::SpawnEnemy() 
{
	// Instantiate a new enemy
	EnemyType1* newEnemy = new EnemyType1();

	// Load Enemy Textures
	static Texture* enemy1Texture = new Texture();
	enemy1Texture->Initialise("../assets/ball.png"); // Replace later

	// Initialise Enemy
	if (newEnemy->Initialise(*m_pRenderer, *enemy1Texture, m_pRenderer->GetWidth(), m_pRenderer->GetHeight())) {
		newEnemy->SpawnOffScreen();
		m_enemies.push_back(newEnemy);
	}
}

void SceneMain::processEnemies(float deltaTime) {
	// Process each enemy
	for (auto& enemy : m_enemies) {
		enemy->Process(deltaTime, m_player.GetPosition());

		float dx = enemy->GetPosition().x - m_player.GetPosition().x;
		float dy = enemy->GetPosition().y - m_player.GetPosition().y;
		float distanceSq = dx * dx + dy * dy;

		float combinedRadius = enemy->GetRadius() + m_player.GetRadius();

		if (distanceSq < (combinedRadius * combinedRadius)) {
			m_player.TakeDamage();
			break; // Only Damage once per frame (for if I increase health later)
		}
	}

	// Remove dead enemies
	m_enemies.erase(
		std::remove_if(m_enemies.begin(), m_enemies.end(),
			[](Enemy* enemy) {
				if (!enemy->IsAlive()) {
					delete enemy;
					return true;
				}
				return false;
			}),
		m_enemies.end());
}

void SceneMain::handleAttackCollisions(float deltaTime) {
	if (m_player.IsAttacking()) {
		Vector2 playerPos = m_player.GetPosition();
		bool facingRight = (m_player.GetFacingDirection() == Player::Direction::Right);

		float attackRange = 350.0f;
		float attackWidth = 75.0f;

		// Flip attack if facing left
		float offsetX = 0.0f;
		if (facingRight) {
			offsetX = m_player.GetRadius();
		}
		else {
			offsetX = -(attackRange + m_player.GetRadius());
		}

		Vector2 attackPos(playerPos.x + offsetX, playerPos.y - (attackWidth / 2));
		float attackLeft = attackPos.x;
		float attackRight = attackPos.x + attackRange;
		float attackTop = attackPos.y;
		float attackBottom = attackPos.y + attackWidth;

		for (Enemy* enemy : m_enemies) {
			Vector2 enemyPos = enemy->GetPosition();
			float enemyRadius = enemy->GetRadius();

			// Check if enemy is within attack range
			if (enemyPos.x + enemyRadius > attackLeft && enemyPos.x - enemyRadius < attackRight &&
				enemyPos.y + enemyRadius > attackTop && enemyPos.y - enemyRadius < attackBottom) 
			{
				enemy->TakeDamage(60 * deltaTime); // Take 60 damage per second
				enemy->SetTakingDamage(true); // Slows enemy and tints red	
			}
			else {
				enemy->SetTakingDamage(false); // Reset speed if not hit
			}
		}
	}
}