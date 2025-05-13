#ifndef SCENEMAIN_H
#define SCENEMAIN_H

// Local includes:
#include "scene.h"
#include "animatedsprite.h"
#include "fmod.hpp"
#include "player.h"
#include "enemytype1.h"
#include "enemytype2.h"
#include "enemytype3.h"
#include "boss.h"
#include "texture.h"
#include "powerupinvincibility.h"
#include "powerupzerooverheat.h"
#include "powerupgenocide.h"

// Library includes:
#include <vector>
#include <memory>

// Forward declarations:
class Renderer;
class Sprite;

// Class declaration:
class SceneMain : public Scene
{
	// Member methods:
public:
	SceneMain();
	virtual ~SceneMain();

	virtual bool Initialise(Renderer& renderer);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	void SpawnEnemy();

	void processEnemies(float deltaTime);
	void handleAttackCollisions(float deltaTime);

	void processPowerups(float deltaTime);

	void KillAllEnemies();

	enum class GameState {
		Playing,
		Paused
	};

	void GameStateCheck(InputSystem& inputSystem);
	GameState GetGameState() const;

	void DrawPauseMenu(Renderer& renderer);
	
	void ResetGame();

	void Progression(float deltaTime);

	void DebugKeys(float deltaTime, InputSystem& inputSystem);

	bool LoadTextures();

protected:
	Player m_player;

private:
	SceneMain(const SceneMain& sceneMain) = delete;
	SceneMain& operator=(const SceneMain& sceneMain) = delete;

	// Member data:
public:
	GameState m_gameState;

protected:
	Renderer* m_pRenderer;

	std::vector<Enemy*> m_enemies;
	std::vector<Texture*> m_textures;

	Texture* m_enemy1Texture;
	Texture* m_enemy2Texture;
	Texture* m_enemy3Texture;
	Texture* m_bossTexture;
	Boss* m_finalBoss;

	const float m_baseEnemySpawnWeights[3] = { 0.5f, 0.0f, 0.0f }; // Weights for enemy types
	float m_enemySpawnWeights[3] = { 0.5f, 0.0f, 0.0f }; // Weights for enemy types

	float m_enemySpawnTimer;
	const float m_baseEnemySpawnInterval = 1.5f;
	float m_enemySpawnInterval;
	float m_gameTimer;
	bool m_bossReached = false;

	Texture* m_playerTexture;

	int m_screenWidth;
	int m_screenHeight;

	std::vector<Powerup*> m_powerups;
	float m_powerupSpawnTimer;

	Sprite* m_pauseOverlaySprite = nullptr;
	Texture* m_pauseOverlayTexture = nullptr;
	bool m_pauseAssetsLoaded = false;

private:
	// IMGui Variables
	bool m_showHitbox = true; // Set to false for release
};

#endif // SCENEMAIN_H