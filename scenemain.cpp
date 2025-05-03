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

	// Process each enemy
	for (auto& enemy : m_enemies) {
		enemy->Process(deltaTime, m_player.GetPosition());
	}
}

void
SceneMain::Draw(Renderer& renderer)
{
	m_player.Draw(renderer);
	
	for (auto& enemy : m_enemies) {
		enemy->Draw(renderer);
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