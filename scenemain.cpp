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
#include "enemytype2.h"
#include "enemytype3.h"
#include "boss.h"
#include "enemy.h"
#include "xboxcontroller.h"


// Library includes:
#include <cassert>
#include "fmod.hpp"
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <algorithm>

SceneMain::SceneMain()
	: m_enemySpawnTimer(0.0f)
	, m_enemySpawnInterval(1.5f) // Default: 1.5
	, m_gameTimer(0.0f)
	, m_powerupSpawnTimer(30.0f) // Default: 30
	, m_playerTexture(0)
{
}

SceneMain::~SceneMain() 
{

	for (Texture* texture : m_textures) {
		delete texture;
	}
	m_textures.clear();

	if (m_pauseOverlaySprite) {
		delete m_pauseOverlaySprite;
		m_pauseOverlaySprite = nullptr;
	}

	for (Powerup* powerup : m_powerups) {
		delete powerup;
	}
	m_powerups.clear();

	for (Enemy* enemy : m_enemies) {
		delete enemy;
	}
	m_enemies.clear();
}

bool
SceneMain::Initialise(Renderer& renderer)
{
	m_pRenderer = &renderer;
	m_screenWidth = renderer.GetWidth();
	m_screenHeight = renderer.GetHeight();

	LoadTextures();

	// Spawn in center
	Vector2 startPos(m_screenWidth / 2, m_screenHeight / 2);
	m_player.SetPosition(startPos);

	m_gameState = GameState::Playing;

	return true;
}

void
SceneMain::Process(float deltaTime, InputSystem& inputSystem) 
{
	// Check for pause
	GameStateCheck(inputSystem);

	if (m_gameState == GameState::Paused) {
		return; // Don't process if paused
	}

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

	processPowerups(deltaTime);

	DebugKeys(deltaTime, inputSystem);

	Progression(deltaTime);

	// Check if player is alive
	if (!m_player.IsAlive()) {
		LogManager::GetInstance().Log("Player is dead!");
		// Lose screen here
	}
}

void
SceneMain::Draw(Renderer& renderer)
{
	m_player.DrawSprite(renderer);
	
	for (auto& enemy : m_enemies) {
		enemy->Draw(renderer);
	}

	for (auto& p : m_powerups)
		p->Draw(renderer);


	// Display Attack Hitbox
	if (m_showHitbox && m_player.CanAttack()) {
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

	m_player.DrawHeatBar(renderer);

	if (m_gameState == GameState::Paused) {
		DrawPauseMenu(renderer);
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
	float totalWeight = m_enemySpawnWeights[0] + m_enemySpawnWeights[1] + m_enemySpawnWeights[2];
	float randValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * totalWeight;
	int enemyType = 0;

	// Randomly choose between enemy types
	if (randValue < m_enemySpawnWeights[0]) {
		enemyType = 1;
	}
	else if (randValue < m_enemySpawnWeights[0] + m_enemySpawnWeights[1]) {
		enemyType = 2;
	}
	else {
		enemyType = 3;
	}

	Enemy* newEnemy = nullptr;

	switch (enemyType) {
	case 1:
		newEnemy = new EnemyType1();

		if (!newEnemy->Initialise(*m_pRenderer, *m_enemy1Texture, m_pRenderer->GetWidth(), m_pRenderer->GetHeight())) {
			LogManager::GetInstance().Log("Enemy failed to initialise!");
			delete newEnemy; // Clean up if failed
		}

		break;

	case 2:
		newEnemy = new EnemyType2();

		if (!newEnemy->Initialise(*m_pRenderer, *m_enemy2Texture, m_pRenderer->GetWidth(), m_pRenderer->GetHeight())) {
			LogManager::GetInstance().Log("Enemy failed to initialise!");
			delete newEnemy; 
		}

		break;

	case 3:
		newEnemy = new EnemyType3();
	
		if (!newEnemy->Initialise(*m_pRenderer, *m_enemy3Texture, m_pRenderer->GetWidth(), m_pRenderer->GetHeight())) {
			LogManager::GetInstance().Log("Enemy failed to initialise!");
			delete newEnemy;
		}

		break;
	
	default:
		LogManager::GetInstance().Log("Invalid enemy type!");
		return; 
	}

	newEnemy->SpawnOffScreen();
	m_enemies.push_back(newEnemy);
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

			if(m_player.GetHealth() == 0) {
				Game::GetInstance().Quit();
			}
			break; // Only damage player once per frame (for if I increase health later)
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

	// Store attack area
	Vector2 attackPos(playerPos.x + offsetX, playerPos.y - (attackWidth / 2));
	float attackLeft = attackPos.x;
	float attackRight = attackPos.x + attackRange;
	float attackTop = attackPos.y;
	float attackBottom = attackPos.y + attackWidth;

	for (Enemy* enemy : m_enemies) {
		Vector2 enemyPos = enemy->GetPosition();
		float enemyRadius = enemy->GetRadius();

		// Check if enemy is within attack area
		if (enemyPos.x + enemyRadius > attackLeft && enemyPos.x - enemyRadius < attackRight &&
			enemyPos.y + enemyRadius > attackTop && enemyPos.y - enemyRadius < attackBottom)
		{
			if (m_player.CanAttack()) {
				enemy->TakeDamage(100 * deltaTime); // Take 100 damage per second
				enemy->SetTakingDamage(true); // Slows enemy and tints red	
			}
		}
		else {
			enemy->SetTakingDamage(false); // Reset speed if not hit
		}
	}
}

void SceneMain::processPowerups(float deltaTime)
{
	m_powerupSpawnTimer -= deltaTime;
	if (m_powerupSpawnTimer <= 0.0f) {
		Vector2 pos(
			static_cast<float>(rand() % (m_screenWidth - 50 + 1) + 50),
			static_cast<float>(rand() % (m_screenHeight - 50 + 1) + 50)
		);

		// Determine type of powerup
		// 1 = Invincibility
		// 2 = No Overheat
		// 3 = Kill All Enemies
		int type = rand() % (3 - 1 + 1) + 1; // (max - min + 1) + min

		switch (type) {
		case 1:
			m_powerups.push_back(new PowerupInvincibility(pos));
			break;
		case 2:
			m_powerups.push_back(new PowerupZeroOverheat(pos));
			break;
		case 3:
			m_powerups.push_back(new PowerupGenocide(pos));
			break;
		}

		m_powerupSpawnTimer = rand() % (60 - 40 + 1) + 40; // Random spawn time between 40 and 60 seconds
	}

	// Despawn powerups
	for (auto it = m_powerups.begin(); it != m_powerups.end(); ) {
		(*it)->Process(deltaTime);  // Update the lifetime

		// If the powerup has expired, delete it
		if ((*it)->IsExpired()) {
			delete* it;
			it = m_powerups.erase(it);
		}
		else {
			++it;
		}
	}

	// Player pickup detection
	for (auto it = m_powerups.begin(); it != m_powerups.end(); ) {
		Vector2 powerupPos = (*it)->GetPosition();
		float dx = powerupPos.x - m_player.GetPosition().x;
		float dy = powerupPos.y - m_player.GetPosition().y;
		float distanceSq = dx * dx + dy * dy;

		float combinedRadius = (*it)->GetRadius() + m_player.GetRadius();

		if (distanceSq < (combinedRadius * combinedRadius)) {
			(*it)->ApplyPowerup(m_player, *this);
			delete* it; // Manual delete
			it = m_powerups.erase(it);
		}
		else {
			++it;
		}
	}
}

void SceneMain::KillAllEnemies() {
	for (Enemy* enemy : m_enemies) {
		enemy->TakeDamage(enemy->GetHealth()); // Kill enemy
	}
}

void SceneMain::GameStateCheck(InputSystem& inputSystem) {
	// Check for pause button
	if ((inputSystem.GetKeyState(SDL_SCANCODE_P) == BS_PRESSED) ||
		(inputSystem.GetController(0) && inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED)) {

		if (m_gameState == GameState::Playing) {
			m_gameState = GameState::Paused;
		}
		else {
			m_gameState = GameState::Playing;
		}
	}

	if (m_gameState == GameState::Paused) {
		// Restart Game
		if (inputSystem.GetKeyState(SDL_SCANCODE_R) == BS_PRESSED ||
			inputSystem.GetController(0) && inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == BS_PRESSED)
		{
			ResetGame();
		}

		// Quit Game
		if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED ||
			inputSystem.GetController(0) && inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_BACK) == BS_PRESSED)
		{
			Game::GetInstance().Quit();
		}
	}
}

SceneMain::GameState SceneMain::GetGameState() const {
	return m_gameState;
}

void SceneMain::DrawPauseMenu(Renderer& renderer) {
	int centerX = m_screenWidth / 2;
	int centerY = m_screenHeight / 2;

	// Draw pause menu background
	renderer.DrawRect(centerX, centerY, renderer.GetWidth(), renderer.GetHeight(), 0.0f, 0.0f, 0.0f, 0.5f);

	// Draw pause menu sprite
	m_pauseOverlaySprite->SetX(centerX);
	m_pauseOverlaySprite->SetY(centerY + m_screenHeight / 16);
	m_pauseOverlaySprite->Draw(renderer);
}

void SceneMain::ResetGame() {
	// Clearing Screen
	for (Enemy* enemy : m_enemies) {
		delete enemy;
	}
	for (Powerup* powerup : m_powerups) {
		delete powerup;
	}
	m_enemies.clear();
	m_powerups.clear();
	
	// Resetting Variables
	m_enemySpawnTimer = 0.0f;
	m_enemySpawnInterval = m_baseEnemySpawnInterval;
	m_gameTimer = 0.0f;
	m_bossReached = false;
	
	// Resetting Spawn Weights
	for (int i = 0; i < 3; ++i) {
		m_enemySpawnWeights[i] = m_baseEnemySpawnWeights[i];
	}

	// Reset Player
	m_player.SetPosition(Vector2(m_screenWidth / 2, m_screenHeight / 2));
	m_player.ResetPlayer();

	// Unpause Game
	m_gameState = GameState::Playing;

}

void SceneMain::Progression(float deltaTime) {
	// Increase difficulty over time
	// MINUTE 1:
	if (m_gameTimer <= 60.0f) {
		m_enemySpawnInterval = m_baseEnemySpawnInterval;		// Default: 1.5
		m_enemySpawnWeights[0] = m_baseEnemySpawnWeights[0];	// Default: 0.5
		m_enemySpawnWeights[1] = m_baseEnemySpawnWeights[1];	// Default: 0.0
		m_enemySpawnWeights[2] = m_baseEnemySpawnWeights[2];	// Default: 0.0
	}

	// MINUTE 2:
	else if (m_gameTimer >= 60 && m_gameTimer <= 120.0f) {
		m_enemySpawnInterval = 1.2f;
		m_enemySpawnWeights[0] = 0.85f;
		m_enemySpawnWeights[1] = 0.15f;
		m_enemySpawnWeights[2] = 0.0f;
	}

	// MINUTE 3:
	else if (m_gameTimer >= 120 && m_gameTimer <= 180.0f) {
		m_enemySpawnInterval = 1.0f;
		m_enemySpawnWeights[0] = 0.75f;
		m_enemySpawnWeights[1] = 0.15f;
		m_enemySpawnWeights[2] = 0.1f;
	}

	// MINUTE 4:
	else if (m_gameTimer >= 180 && m_gameTimer <= 240.0f) {
		m_enemySpawnInterval = 0.8f;
		m_enemySpawnWeights[0] = 0.65f;
		m_enemySpawnWeights[1] = 0.20f;
		m_enemySpawnWeights[2] = 0.15f;
	}

	// MINUTE 5:
	else if (m_gameTimer >= 240 && m_gameTimer <= 300.0f) {
		m_enemySpawnInterval = 0.7f;
		m_enemySpawnWeights[0] = 0.55f;
		m_enemySpawnWeights[1] = 0.25f;
		m_enemySpawnWeights[2] = 0.2f;
	}

	// BOSS FIGHT:
	else if (m_gameTimer >= 300.0f) {
		if (!m_bossReached) {
			for (Enemy* enemy : m_enemies) {
				delete enemy;
			}

			m_enemies.clear();
			m_enemySpawnInterval = 5.0f;

			// Spawn Boss
			m_finalBoss = new Boss();

			if (!m_finalBoss->Initialise(*m_pRenderer, *m_bossTexture, m_pRenderer->GetWidth(), m_pRenderer->GetHeight())) {
				LogManager::GetInstance().Log("Boss failed to initialise!");
				delete m_finalBoss;
			}

			m_finalBoss->SpawnOffScreen();
			m_enemies.push_back(m_finalBoss);
			m_bossReached = true;
		}

		if (!m_finalBoss->IsAlive()) {
			//stop timer
			//calculate score
			//pause game
			//win screen
		}
	}
}

void SceneMain::DebugKeys(float deltaTime, InputSystem& inputSystem)
{
	// GOD MODE - F1
	// ZERO OVERHEAT - F2
	// KILL ALL ENEMIES - F3
	// SKIP TIMER - F4
	if (inputSystem.GetKeyState(SDL_SCANCODE_F4) == BS_PRESSED)
	{
		m_gameTimer = std::ceil(m_gameTimer / 60.0f) * 60.0f;

	}
	// DISPLAY ATTACK HITBOX - F5
	// RESTART GAME - F12
	if (inputSystem.GetKeyState(SDL_SCANCODE_F12) == BS_PRESSED)
	{
		ResetGame();
	}

}

bool
SceneMain::LoadTextures() {
	// Load Pause Textures
	m_pauseOverlayTexture = new Texture();
	if (!m_pauseOverlayTexture->Initialise("../assets/pause.png")) {
		LogManager::GetInstance().Log("Failed to load pause overlay texture!");
		delete m_pauseOverlayTexture;
		m_pauseOverlayTexture = nullptr;
		return false;
	}
	m_pauseOverlaySprite = new Sprite();
	if (!m_pauseOverlaySprite->Initialise(*m_pauseOverlayTexture)) {
		LogManager::GetInstance().Log("Failed to load pause overlay sprite!");
		delete m_pauseOverlaySprite;
		m_pauseOverlaySprite = nullptr;
		return false;
	}
	float targetWidth = m_screenWidth * 0.8;
	float targetHeight = m_screenHeight * 0.8;
	float scaleX = targetWidth / m_pauseOverlayTexture->GetWidth();
	float scaleY = targetHeight / m_pauseOverlayTexture->GetHeight();
	float scale = std::min(scaleX, scaleY); // Keep aspect ratio
	m_pauseOverlaySprite->SetScale(scale);

	m_textures.push_back(m_pauseOverlayTexture);

	// Load Player Texture
	m_playerTexture = new Texture();
	if (!m_playerTexture->Initialise("../assets/ball.png")) { // Replace later
		LogManager::GetInstance().Log("Failed to load player texture!");
		delete m_playerTexture;
		m_playerTexture = nullptr;
		return false;
	}
	m_player.Initialise(*m_pRenderer, *m_playerTexture);
	m_textures.push_back(m_playerTexture);

	// Load Enemy Textures
	m_enemy1Texture = new Texture();
	if (!m_enemy1Texture->Initialise("../assets/ball.png"))
	{
		LogManager::GetInstance().Log("Failed to load enemy1 texture!");
		delete m_enemy1Texture;
		m_enemy1Texture = nullptr;
		return false;
	}
	m_textures.push_back(m_enemy1Texture);

	m_enemy2Texture = new Texture();
	if (!m_enemy2Texture->Initialise("../assets/ball.png"))
	{
		LogManager::GetInstance().Log("Failed to load enemy2 texture!");
		delete m_enemy2Texture;
		m_enemy2Texture = nullptr;
		return false;
	}
	m_textures.push_back(m_enemy2Texture);

	m_enemy3Texture = new Texture();
	if (!m_enemy3Texture->Initialise("../assets/ball.png"))
	{
		LogManager::GetInstance().Log("Failed to load enemy3 texture!");
		delete m_enemy3Texture;
		m_enemy3Texture = nullptr;
		return false;
	}
	m_textures.push_back(m_enemy3Texture);

	m_bossTexture = new Texture();
	if (!m_bossTexture->Initialise("../assets/ball.png"))
	{
		LogManager::GetInstance().Log("Failed to load boss texture!");
		delete m_bossTexture;
		m_bossTexture = nullptr;
		return false;
	}
	m_textures.push_back(m_bossTexture);

	return true;
}