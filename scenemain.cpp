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
#include "audiomanager.h"


// Library includes:
#include <cassert>
#include "fmod.hpp"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <string>

SceneMain::SceneMain()
	: m_enemySpawnTimer(0.0f)
	, m_enemySpawnInterval(1.5f) // Default: 1.5
	, m_gameTimer(0.0f)
	, m_powerupSpawnTimer(30.0f) // Default: 30
	, m_bossSprite(nullptr)
	, m_enemy1Sprite(nullptr)
	, m_enemy2Sprite(nullptr)
	, m_enemy3Sprite(nullptr)
	, m_finalBoss(nullptr)
	, m_finalTime(0.0f)
	, m_flame(nullptr)
	, m_gameState(GameState::Playing)
	, m_pRenderer(nullptr)
	, m_scoreSprite(nullptr)
	, m_screenHeight(0)
	, m_screenWidth(0)
	, m_winSprite(nullptr)
	, m_disablePlayerInput(false)
{
}

SceneMain::~SceneMain() 
{
	if (m_pauseOverlaySprite) {
		delete m_pauseOverlaySprite;
		m_pauseOverlaySprite = nullptr;
	}

	if (m_scoreSprite) {
		delete m_scoreSprite;
		m_scoreSprite = nullptr;
	}

	if (m_backgroundSprite) {
		delete m_backgroundSprite;
		m_backgroundSprite = nullptr;
	}

	if (m_winSprite) {
		delete m_winSprite;
		m_winSprite = nullptr;
	}

	for (Powerup* powerup : m_powerups) {
		delete powerup;
	}
	m_powerups.clear();

	m_particleManager->Clear();

	if (m_finalBoss) {
		auto it = std::find(m_enemies.begin(), m_enemies.end(), m_finalBoss);
		if (it != m_enemies.end()) {
			m_enemies.erase(it);
		}
		delete m_finalBoss;
		m_finalBoss = nullptr;
	}

	for (Enemy* enemy : m_enemies) {
		delete enemy;
	}
	m_enemies.clear();

	for (auto& pair : m_digitSprites)
	{
		delete pair.second;
	}
	m_digitSprites.clear();

	AudioManager::GetInstance().Shutdown();
}

bool
SceneMain::Initialise(Renderer& renderer)
{
	m_pRenderer = &renderer;
	m_screenWidth = renderer.GetWidth();
	m_screenHeight = renderer.GetHeight();

	AudioManager::GetInstance().Initialise();
	m_particleManager = std::make_unique<ParticleManager>(renderer);

	LoadTextures();
	LoadAudio();

	// Start Music
	AudioManager::GetInstance().PlaySound("music", 1.0);

	// Spawn in center
	Vector2 startPos(static_cast<float>(m_screenWidth / 2), static_cast<float>(m_screenHeight / 2));
	m_player.SetPosition(startPos);

	m_gameState = GameState::Playing;

	return true;
}

void
SceneMain::Process(float deltaTime, InputSystem& inputSystem) 
{
	// Check for pause
	GameStateCheck(inputSystem);

	if (m_gameState == GameState::Win) {
		if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED ||
			(inputSystem.GetController(0) && inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_BACK) == BS_PRESSED))
		{
			m_flame->SetFlipX(false);
			ResetGame();
		}
		return; // Don't process if paused
	}

	if (m_gameState == GameState::Paused) {
		return; // Don't process if paused
	}

	if (!m_disablePlayerInput) {
		m_player.Process(deltaTime, inputSystem, *m_pRenderer);
	}

	m_gameTimer += deltaTime;
	flameParticleTimer += deltaTime;

	// Spawn Enemies
	m_enemySpawnTimer += deltaTime;
	if (m_enemySpawnTimer >= m_enemySpawnInterval) {
		SpawnEnemy();
		m_enemySpawnTimer = 0.0f;
	}

	AudioManager::GetInstance().Process();

	m_particleManager->Update(deltaTime);

	ProcessWeaponAudio();

	processEnemies(deltaTime);

	handleAttackCollisions(deltaTime);

	processPowerups(deltaTime);

	DebugKeys(deltaTime, inputSystem);

	Progression(deltaTime);

	ProcessDeath(deltaTime);
}

void
SceneMain::Draw(Renderer& renderer)
{
	// Drawing Background
	m_backgroundSprite->SetX(m_screenWidth / 2);
	m_backgroundSprite->SetY(m_screenHeight / 2);
	m_backgroundSprite->SetFlipX(false);

	m_backgroundSprite->Draw(renderer);

	// Drawing Player
	m_player.DrawSprite(renderer);

	// Drawing Enemies
	for (auto& enemy : m_enemies) {
		enemy->Draw(renderer);
	}

	// Drawing Powerups
	for (auto& p : m_powerups)
		p->Draw(renderer);

	// Drawing Flames
	m_flame = m_player.GetFlameSprite();
	Vector2 playerPos = m_player.GetPosition();
	bool facingRight = (m_player.GetFacingDirection() == Player::Direction::Right);
	const float attackRange = 350.0f;
	const float attackWidth = 75.0f;


	// Flip attack if facing left
	int offsetX = 0;
	if (facingRight) {
		offsetX = m_player.GetRadius();
	}
	else {
		offsetX = -(static_cast<int>(attackRange) + m_player.GetRadius());
	}

	Vector2 attackPos(playerPos.x + offsetX, playerPos.y - (attackWidth / 3));

	if (m_player.m_isAttacking == true && m_player.CanAttack()) {
		int centerX = static_cast<int>(attackPos.x + attackRange);
		int centerY = static_cast<int>(attackPos.y + attackWidth);

		int flameX = (centerX - m_flame->GetWidth() / 2) - 12;
		if (!facingRight) {
			flameX -= 10; 
		}

		int flameY = centerY - m_flame->GetHeight() / 2;

		m_flame->SetX(flameX);
		m_flame->SetY(flameY);
		m_flame->SetFlipX(!facingRight);
		m_flame->Draw(*m_pRenderer);

		if (flameParticleTimer >= flameParticleDelay) {
			m_particleManager->SpawnParticles(ParticleType::Fire, Vector2(static_cast<float>(flameX), static_cast<float>(flameY)), 1);
			flameParticleTimer = 0;
		}
	}
	else {
		m_flame->SetFlipX(facingRight);
	}

	// Drawing Attack Hitbox
	if (m_showHitbox && m_player.CanAttack()) {
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		ImVec2 topLeft(attackPos.x, attackPos.y);
		ImVec2 bottomRight(attackPos.x + attackRange, attackPos.y + attackWidth);

		drawList->AddRectFilled(topLeft, bottomRight, IM_COL32(255, 0, 0, 180));
	}

	m_particleManager->Draw(*m_pRenderer);

	DrawTimer();

	DrawScore();

	m_player.DrawHeatBar(renderer);

	// Drawing Menus
	if (m_gameState == GameState::Paused) {
		m_player.m_isAttacking = false;
		DrawPauseMenu(renderer);
		return;
	}

	if (m_gameState == GameState::Win) {
		m_player.m_isAttacking = false;
		DrawWinMenu(renderer);
		return;
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

	ImGui::Separator();
	ImGui::Text("Debug Actions:");

	if (ImGui::Button("God Mode (F1)"))
	{
		m_player.SetHealth(9999999);
		m_player.m_sprite->SetRedTint(1.0f);
		m_player.m_sprite->SetGreenTint(0.0f);
		m_player.m_sprite->SetBlueTint(0.0f);
	}
	if (ImGui::Button("Zero Overheat (F2)"))
	{
		PowerupZeroOverheat* powerup = new PowerupZeroOverheat(Vector2(m_player.GetPosition().x, m_player.GetPosition().y));
		m_powerups.push_back(powerup);
		//powerup->ApplyPowerup(m_player, *this);
	}
	if (ImGui::Button("Kill All Enemies (F3)"))
	{
		DebugKillAllEnemies();
	}
	if (ImGui::Button("Skip Timer to Next Minute (F4)"))
	{
		m_gameTimer = std::ceil(m_gameTimer / 60.0f) * 60.0f;
	}
	if (ImGui::Button("Toggle Hitbox Display (F5)"))
	{
		m_showHitbox = !m_showHitbox;
	}
	if (ImGui::Button("Toggle Fullscreen (F11)"))
	{
		m_pRenderer->ToggleFullscreen();
	}
	if (ImGui::Button("Restart Game (F12)"))
	{
		ResetGame();
		return;
	}
}

void SceneMain::DrawTimer()
{
	int minutes = static_cast<int>(m_gameTimer) / 60;
	int seconds = static_cast<int>(m_gameTimer) % 60;

	char buf[16];
	std::snprintf(buf, sizeof(buf), "%02d:%02d", minutes, seconds);

	ImDrawList* dl = ImGui::GetForegroundDrawList();

	ImVec2 pos = ImVec2(25.0f, 25.0f);

	ImFont* font = m_pRenderer->m_bigFont;
	float font_sz = 64.0f;

	ImU32 col = IM_COL32(255, 255, 255, 255);

	if (m_gameTimer >= 300) {
		col = IM_COL32(255, 0, 0, 255);
	}

	dl->AddText(font, font_sz, pos, col, buf);
}

void SceneMain::DrawScore()
{
	int score = m_player.GetScore();

	char scoreBuf[32];
	std::snprintf(scoreBuf, sizeof(scoreBuf), "%06d", score);

	const float xPos = 25.0f;
	const float yTimer = 25.0f;
	const float timerSize = 64.0f;
	const float lineGap = 8.0f;
	float       yScore = yTimer + timerSize + lineGap;

	ImDrawList* dl = ImGui::GetForegroundDrawList();
	ImFont* font = m_pRenderer->m_mediumFont;
	float font_sz = 36.0f;
	ImU32 col = IM_COL32(255, 255, 255, 255);

	dl->AddText(font, font_sz, ImVec2(xPos, yScore), col, scoreBuf);
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
	int colour = rand() % (2 - 1 + 1) + 1; // (max - min + 1) + min

	switch (enemyType) {
	case 1:
		newEnemy = new EnemyType1();
		
		if (colour == 1) {
			newEnemy->Initialise(*m_pRenderer, "../assets/green_slime.png", m_screenWidth, m_screenHeight);
		}
		else {
			newEnemy->Initialise(*m_pRenderer, "../assets/red_slime.png", m_screenWidth, m_screenHeight);
		}

		break;

	case 2:
		newEnemy = new EnemyType2();
		newEnemy->Initialise(*m_pRenderer, "../assets/bat_fly.png", m_screenWidth, m_screenHeight);
		break;

	case 3:
		newEnemy = new EnemyType3();
		newEnemy->Initialise(*m_pRenderer, "../assets/bee.png", m_screenWidth, m_screenHeight);
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

		int combinedRadius = enemy->GetRadius() + m_player.GetRadius();

		if (distanceSq < (combinedRadius * combinedRadius)) {
			m_player.TakeDamage();
			m_player.m_isAttacking = false;
	
			break; // Only damage player once per frame (for if I increase health later)
		}
	}

	// Remove dead enemies
	for (auto it = m_enemies.begin(); it != m_enemies.end(); ) {
		Enemy* enemy = *it;

		if (!enemy->IsAlive()) { // If enemy dead
			if (!m_skipScore) // If enemy dies from powerup
			{
				m_player.EditScore(enemy->GetPointsValue()); // Add score
			}
		
			// If enemy is boss
			if (dynamic_cast<Boss*>(enemy)) { 
				if (!m_bossDeathTriggered) {
					m_bossDeathTriggered = true;
					m_bossDeathTimer = 0.0f;
				}
			}

			AudioManager::GetInstance().PlaySound("plop", 1.0f);

			delete enemy;
			it = m_enemies.erase(it);
		}
		else {
			++it;
		}
	}

	m_skipScore = false;
}

void SceneMain::handleAttackCollisions(float deltaTime) {
	Vector2 playerPos = m_player.GetPosition();
	bool facingRight = (m_player.GetFacingDirection() == Player::Direction::Right);

	int attackRange = 350;
	int attackWidth = 75;

	// Flip attack if facing left
	int offsetX = 0;
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
		int enemyRadius = enemy->GetRadius();

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

		// Spawn Particles
		m_particleManager->SpawnParticles(ParticleType::Powerup, pos, 100);

		m_powerupSpawnTimer = static_cast<float>(rand() % (60 - 40 + 1) + 40); // Random spawn time between 40 and 60 seconds
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
			AudioManager::GetInstance().PlaySound("powerup", 1.0f);
			(*it)->ApplyPowerup(m_player, *this);
			m_player.EditScore((*it)->GetPointsValue());
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
		if (dynamic_cast<Boss*>(enemy) == nullptr) {
			// Not a boss, kill it
			enemy->TakeDamage(enemy->GetHealth());
		}
	}
}

// Only runs with F3, kills boss as well.
void SceneMain::DebugKillAllEnemies() {
	m_skipScore = true;

	for (Enemy* enemy : m_enemies) {
		enemy->TakeDamage(enemy->GetHealth());
	}
}

void SceneMain::GameStateCheck(InputSystem& inputSystem) {
	// Check for pause button
	if (((inputSystem.GetKeyState(SDL_SCANCODE_P) == BS_PRESSED) ||
		(inputSystem.GetController(0) && inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED)) &&
		m_gameState != GameState::Win) {

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
			return;
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
	int centerX = renderer.GetWidth() / 2;
	int centerY = renderer.GetHeight() / 2;

	// Draw pause menu background
	renderer.DrawRect(centerX, centerY, static_cast<float>(renderer.GetWidth()), static_cast<float>(renderer.GetHeight()), 0.0f, 0.0f, 0.0f, 0.5f);

	// Draw pause menu sprite
	m_pauseOverlaySprite->SetX(centerX);
	m_pauseOverlaySprite->SetY(centerY + m_screenHeight / 18);
	m_pauseOverlaySprite->Draw(renderer);
}

void SceneMain::DrawWinMenu(Renderer& renderer) {
	int centerX = m_screenWidth / 2;
	int centerY = m_screenHeight / 2;

	// Draw pause menu background
	renderer.DrawRect(centerX, centerY, static_cast<float>(renderer.GetWidth()), static_cast<float>(renderer.GetHeight()), 1.0f, 0.75f, 0.8f, 0.5f);

	if (m_winSprite) {
		m_winSprite->SetX(centerX);
		m_winSprite->SetY(centerY);
		m_winSprite->Draw(renderer);
	}

	if (m_scoreSprite) {
		m_scoreSprite->Draw(renderer);
	}
}

void SceneMain::ProcessDeath(float deltaTime) {
	if (m_player.GetHealth() <= 0) {
		m_playerDeathTimer += deltaTime;
		m_enemySpawnInterval = 0.0f;

		AudioManager::GetInstance().StopSound("boss_music");
		
		if (!loseAudioPlaying) {
			AudioManager::GetInstance().PlaySound("lose", 1.0f);
			loseAudioPlaying = true;
		}

		if (m_playerDeathTimer >= 4.0f) {
			m_flame->SetFlipX(false);
			ResetGame();
			return;
		}
	}
}

void SceneMain::ResetGame() {
	// Clearing Screen
	if (m_finalBoss) {
		auto it = std::find(m_enemies.begin(), m_enemies.end(), m_finalBoss);
		if (it != m_enemies.end()) {
			m_enemies.erase(it);
		}
		//delete m_finalBoss;
		m_finalBoss = nullptr;
	}

	for (Enemy* enemy : m_enemies) {
		if (enemy != nullptr && enemy != m_finalBoss) {
			delete enemy;
		}
	}
	for (Powerup* powerup : m_powerups) {
		delete powerup;
	}
	m_enemies.clear();
	m_powerups.clear();
	m_particleManager->Clear();


	// Delete score sprite
	if (m_scoreSprite) {
		delete m_scoreSprite;
		m_scoreSprite = nullptr;
	}

	// Resetting Variables
	m_enemySpawnTimer = 0.0f;
	m_enemySpawnInterval = m_baseEnemySpawnInterval;
	m_gameTimer = 0.0f;
	m_finalTime = 0.0f;
	m_finalScore = 0;
	m_bossReached = false;
	m_bossDeathAudioPlaying = false;
	m_bossDeathTriggered = false;
	m_bossDeathTimer = 0.0f;
	m_playerDeathTimer = 0.0f;
	enemiesCleared = false;
	m_skipScore = false;
	loseAudioPlaying = false;
	m_disablePlayerInput = false;
	
	// Resetting Spawn Weights
	for (int i = 0; i < 3; ++i) {
		m_enemySpawnWeights[i] = m_baseEnemySpawnWeights[i];
	}

	// Reset Player
	m_player.SetPosition(Vector2(static_cast<float>(m_screenWidth / 2), static_cast<float>(m_screenHeight / 2)));
	m_player.ResetPlayer();

	// Reset Music
	AudioManager::GetInstance().StopSound("boss_music");
	if (!musicPlaying) {
		AudioManager::GetInstance().PlaySound("music", 1.0f);
	}
	bossMusicPlaying = false;
	musicPlaying = true;

	// Unpause Game
	m_gameState = GameState::Playing;

}

void SceneMain::Progression(float deltaTime) {
	// Increase difficulty over time
	// MINUTE 1:
	if (m_gameTimer <= 60.0f) {
		m_enemySpawnInterval = m_baseEnemySpawnInterval;		// Default: 1.7
		m_enemySpawnWeights[0] = m_baseEnemySpawnWeights[0];	// Default: 0.95
		m_enemySpawnWeights[1] = m_baseEnemySpawnWeights[1];	// Default: 0.00
		m_enemySpawnWeights[2] = m_baseEnemySpawnWeights[2];	// Default: 0.00
	}

	// MINUTE 2:
	else if (m_gameTimer >= 60 && m_gameTimer <= 120.0f) {
		m_enemySpawnInterval = 1.4f;
		m_enemySpawnWeights[0] = 0.85f;
		m_enemySpawnWeights[1] = 0.15f;
		m_enemySpawnWeights[2] = 0.00f;
	}

	// MINUTE 3:
	else if (m_gameTimer >= 120 && m_gameTimer <= 180.0f) {
		m_enemySpawnInterval = 1.2f;
		m_enemySpawnWeights[0] = 0.75f;
		m_enemySpawnWeights[1] = 0.15f;
		m_enemySpawnWeights[2] = 0.1f;
	}

	// MINUTE 4:
	else if (m_gameTimer >= 180 && m_gameTimer <= 240.0f) {
		m_enemySpawnInterval = 1.0f;
		m_enemySpawnWeights[0] = 0.65f;
		m_enemySpawnWeights[1] = 0.20f;
		m_enemySpawnWeights[2] = 0.15f;
	}

	// MINUTE 5:
	else if (m_gameTimer >= 240 && m_gameTimer <= 300.0f) {
		m_enemySpawnInterval = 0.8f;
		m_enemySpawnWeights[0] = 0.60f;
		m_enemySpawnWeights[1] = 0.23f;
		m_enemySpawnWeights[2] = 0.17f;
	}

	// BOSS FIGHT:
	else if (m_gameTimer >= 300.0f) {
		if (!m_bossReached) {
			for (Enemy* enemy : m_enemies) {
				delete enemy;
			}

			if (!bossMusicPlaying) {
				AudioManager::GetInstance().StopSound("music");
				AudioManager::GetInstance().PlaySound("boss_music", 1.0f);
				bossMusicPlaying = true;
				musicPlaying = false;
			}

			m_enemies.clear();
			m_enemySpawnInterval = 3.0f;

			// Spawn Boss
			m_finalBoss = new Boss();

			if (!m_finalBoss->Initialise(*m_pRenderer, "../assets/boss.png", m_screenWidth, m_screenHeight)) {
				LogManager::GetInstance().Log("Boss failed to initialise!");
				delete m_finalBoss;
			}

			m_finalBoss->SpawnOffScreen();
			m_enemies.push_back(m_finalBoss);
			m_bossReached = true;
		}

		// Boss death delay
		if (m_bossDeathTriggered) {
			m_bossDeathTimer += deltaTime;
			m_enemySpawnInterval = 0.0f;
			m_skipScore = true;
			m_player.SetInvincible();

			if (!enemiesCleared)
			{
				KillAllEnemies();
				enemiesCleared = true;
			}

			if (!m_bossDeathAudioPlaying) {

				AudioManager::GetInstance().StopSound("boss_music");
				AudioManager::GetInstance().PlaySound("boss_death", 1.0f);
				m_bossDeathAudioPlaying = true;
			}

			if (m_bossDeathTimer >= m_bossDeathDelay) {
				m_finalTime = m_gameTimer;
				m_finalScore = m_player.GetScore();

				// Create text with final score
				if (!m_scoreSprite) {
					std::ostringstream oss;
					oss << std::setw(6)           // pad to at least 6 characters
						<< std::setfill('0')      // filling character is '0'
						<< m_finalScore;              // the number

					std::string scoreText = "Score: " + oss.str();
					m_pRenderer->CreateStaticText(scoreText.c_str(), 72);
					m_scoreSprite = m_pRenderer->CreateSprite(scoreText.c_str());
				}

				m_scoreSprite->SetX(m_screenWidth / 2);
				m_scoreSprite->SetY(m_screenHeight / 2);

				AudioManager::GetInstance().PlaySound("victory", 1.0f);

				m_gameState = GameState::Win; // Stop Processing
			}
		}
	}
}

void SceneMain::DebugKeys(float deltaTime, InputSystem& inputSystem)
{
	// GOD MODE - F1
	if (inputSystem.GetKeyState(SDL_SCANCODE_F1) == BS_PRESSED)
	{
		m_player.SetHealth(9999999);
		m_player.SetInvincible();
	}
	// ZERO OVERHEAT - F2
	if (inputSystem.GetKeyState(SDL_SCANCODE_F2) == BS_PRESSED)
	{
		auto* powerup = new PowerupZeroOverheat(Vector2(0, 0));
		powerup->ApplyPowerup(m_player, *this);
		delete powerup;
	}
	// KILL ALL ENEMIES - F3
	if (inputSystem.GetKeyState(SDL_SCANCODE_F3) == BS_PRESSED)
	{
		DebugKillAllEnemies();
	}
	// SKIP TIMER - F4
	if (inputSystem.GetKeyState(SDL_SCANCODE_F4) == BS_PRESSED)
	{
		m_gameTimer = std::ceil(m_gameTimer / 60.0f) * 60.0f;

	}
	// DISPLAY ATTACK HITBOX - F5
	if (inputSystem.GetKeyState(SDL_SCANCODE_F5) == BS_PRESSED)
	{
		m_showHitbox = !m_showHitbox;
	}
	// FULLSCREEN - F11
	if (inputSystem.GetKeyState(SDL_SCANCODE_F11) == BS_PRESSED)
	{
		m_pRenderer->ToggleFullscreen();
	}
	// RESTART GAME - F12
	if (inputSystem.GetKeyState(SDL_SCANCODE_F12) == BS_PRESSED)
	{
		ResetGame();
		return;
	}
}

bool
SceneMain::LoadTextures() {
	// Load Pause Sprite
	m_pauseOverlaySprite = m_pRenderer->CreateSprite("../assets/pause.png");

	float targetWidth = static_cast<float>(m_screenWidth) * 1.0f;
	float targetHeight = static_cast<float>(m_screenHeight) * 1.0f;
	float scaleX = targetWidth / m_pauseOverlaySprite->GetWidth();
	float scaleY = targetHeight / m_pauseOverlaySprite->GetHeight();
	float scale = std::min(scaleX, scaleY); // Keep aspect ratio
	m_pauseOverlaySprite->SetScale(scale * 0.8f);

	// Load Background Sprite
	m_backgroundSprite = m_pRenderer->CreateSprite("../assets/background.png");
	m_backgroundSprite->SetScale(scale * 1.2f);

	m_player.Initialise(*m_pRenderer);

	// Load Win Sprite Once
	if (!m_winSprite) {
		m_winSprite = m_pRenderer->CreateSprite("../assets/win.png");
		float targetWidth = m_screenWidth * 0.75f;
		float targetHeight = m_screenHeight * 0.75f;
		float scaleX = targetWidth / m_winSprite->GetWidth();
		float scaleY = targetHeight / m_winSprite->GetHeight();
		float scale = std::min(scaleX, scaleY);
		m_winSprite->SetScale(scale);
		if (!m_winSprite) {
			LogManager::GetInstance().Log("Failed to load win.png");
		}
	}

	return true;
}

void
SceneMain::LoadAudio() 
{
	AudioManager::GetInstance().LoadSound("plop", "../assets/sound/plop.mp3", false);
	AudioManager::GetInstance().LoadSound("flamethrower", "../assets/sound/flamethrower.wav", false);
	AudioManager::GetInstance().LoadSound("overheat", "../assets/sound/overheat.mp3", false);
	AudioManager::GetInstance().LoadSound("powerup", "../assets/sound/powerup.mp3", false);
	AudioManager::GetInstance().LoadSound("lose", "../assets/sound/lose.mp3", false);
	AudioManager::GetInstance().LoadSound("boss_death", "../assets/sound/boss_death.mp3", false);
	AudioManager::GetInstance().LoadSound("victory", "../assets/sound/victory.mp3", false);
	AudioManager::GetInstance().LoadSound("music", "../assets/sound/music.mp3", true);
	AudioManager::GetInstance().LoadSound("boss_music", "../assets/sound/boss_music.mp3", true);
}
	
void
SceneMain::ProcessWeaponAudio() 
{
	// Play Weapon Audio
	if (m_player.m_isAttacking && m_player.CanAttack()) {
		if (!weaponAudioPlaying) {
			AudioManager::GetInstance().PlaySound("flamethrower", 0.7f);
			weaponAudioPlaying = true;
		}
	}
	else {
		AudioManager::GetInstance().StopSound("flamethrower");
		weaponAudioPlaying = false;
	}

	// Overheat Audio
	if (m_player.m_isAttacking && m_player.m_isOverheated) {
		if (!overheatAudioPlaying) {
			AudioManager::GetInstance().PlaySound("overheat", 1.0f);
			overheatAudioPlaying = true;
		}
	}
	else {
		overheatAudioPlaying = false;
	}
}