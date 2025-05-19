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
#include "particlemanager.h"

// Library includes:
#include <vector>
#include <memory>
#include <map>

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
	void DrawTimer();
	void DrawScore();
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	void SpawnEnemy();

	void processEnemies(float deltaTime);
	void handleAttackCollisions(float deltaTime);

	void processPowerups(float deltaTime);

	void KillAllEnemies();
	void DebugKillAllEnemies();

	enum class GameState {
		Playing,
		Paused,
		Win
	};

	void GameStateCheck(InputSystem& inputSystem);
	GameState GetGameState() const;

	void DrawPauseMenu(Renderer& renderer);
	void DrawWinMenu(Renderer& renderer);
	
	void ProcessDeath(float deltaTime);

	void ResetGame();

	void Progression(float deltaTime);

	void DebugKeys(float deltaTime, InputSystem& inputSystem);

	bool LoadTextures();
	void LoadAudio();

	void ProcessWeaponAudio();

protected:
	Player m_player;

private:
	SceneMain(const SceneMain& sceneMain) = delete;
	SceneMain& operator=(const SceneMain& sceneMain) = delete;

	// Member data:
public:
	GameState m_gameState;
	bool m_skipScore = false;

protected:
	Renderer* m_pRenderer;

	std::vector<Enemy*> m_enemies;

	Sprite* m_enemy1Sprite;
	Sprite* m_enemy2Sprite;
	Sprite* m_enemy3Sprite;
	Sprite* m_bossSprite;
	Boss* m_finalBoss;
	Sprite* m_backgroundSprite = nullptr;
	Sprite* m_winSprite;

	const float m_baseEnemySpawnWeights[3] = { 0.5f, 0.0f, 0.0f }; // Weights for enemy types
	float m_enemySpawnWeights[3] = { 0.5f, 0.0f, 0.0f }; // Weights for enemy types

	float m_enemySpawnTimer;
	const float m_baseEnemySpawnInterval = 2.0f;
	float m_enemySpawnInterval;

	float m_gameTimer;
	float m_finalTime;
	int m_finalScore = 0;
	Sprite* m_scoreSprite;

	bool m_bossReached = false;

	AnimatedSprite* m_flame;

	bool musicPlaying = true;
	bool bossMusicPlaying = false;

	int m_screenWidth;
	int m_screenHeight;

	std::vector<Powerup*> m_powerups;
	float m_powerupSpawnTimer;

	Sprite* m_pauseOverlaySprite = nullptr;
	Texture* m_pauseOverlayTexture = nullptr;
	bool m_pauseAssetsLoaded = false;

	bool weaponAudioPlaying = false;
	bool overheatAudioPlaying = false;
	bool loseAudioPlaying = false;

	float m_playerDeathTimer = 0.0f;

	bool m_bossDeathTriggered = false;
	float m_bossDeathTimer = 0.0f;
	const float m_bossDeathDelay = 7.5f; 
	bool m_bossDeathAudioPlaying = false;
	bool enemiesCleared = false;

	std::unique_ptr<ParticleManager> m_particleManager;
	float flameParticleTimer = 0.0f;
	float flameParticleDelay = 0.4f;

	std::map<char, Sprite*> m_digitSprites;

	bool m_disablePlayerInput = false;

private:
	// IMGui Variables
	bool m_showHitbox = false;
};

#endif // SCENEMAIN_H