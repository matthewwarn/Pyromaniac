// This include:
#include "scenesplash.h"

// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "imgui/imgui.h"
#include "inputsystem.h"
#include "game.h"
#include "logmanager.h"
#include "xboxcontroller.h"

// Library includes:
#include <cassert>
#include "fmod.hpp"
#include <ctime>
#include <iostream>
#include <cstdlib>
#include <algorithm>

SceneSplash::SceneSplash()
{
}

SceneSplash::~SceneSplash()
{
	for (Texture* texture : m_textures) {
		delete texture;
	}
	m_textures.clear();

	for (Sprite* sprite : m_sprites) {
		delete sprite;
	}
	m_sprites.clear();
}

bool
SceneSplash::Initialise(Renderer& renderer)
{
	m_pRenderer = &renderer;
	m_screenWidth = renderer.GetWidth();
	m_screenHeight = renderer.GetHeight();

	// Load the background texture
	Texture* backgroundTexture = new Texture();
	if (!backgroundTexture->Initialise("../assets/black.png")) {
		LogManager::GetInstance().Log("Failed to load splash background texture!");
		delete backgroundTexture;
		return false;
	}
	m_textures.push_back(backgroundTexture);

	m_backgroundTexture = backgroundTexture;

	m_backgroundSprite = new Sprite();
	if (!m_backgroundSprite->Initialise(*backgroundTexture)) {
		LogManager::GetInstance().Log("Failed to create splash background sprite!");
		delete m_backgroundSprite;
		m_backgroundSprite = nullptr;
		delete backgroundTexture;
		return false;
	}
	m_sprites.push_back(m_backgroundSprite);

	// Load the AUT texture
	m_autTexture = new Texture();
	if (!m_autTexture->Initialise("../assets/aut.png")) {
		LogManager::GetInstance().Log("Failed to load AUT texture!");
		delete m_autTexture;
		return false;
	}
	m_textures.push_back(m_autTexture);

	m_autSprite = new Sprite();
	if (!m_autSprite->Initialise(*m_autTexture)) {
		LogManager::GetInstance().Log("Failed to create AUT sprite!");
		delete m_autSprite;
		m_autSprite = nullptr;
		delete m_autTexture;
		return false;
	}
	m_sprites.push_back(m_autSprite);

	// Load the FMOD texture
	m_fmodTexture = new Texture();
	if (!m_fmodTexture->Initialise("../assets/fmod_white.png")) {
		LogManager::GetInstance().Log("Failed to load FMOD texture!");
		delete m_fmodTexture;
		return false;
	}
	m_textures.push_back(m_fmodTexture);

	m_fmodSprite = new Sprite();
	if (!m_fmodSprite->Initialise(*m_fmodTexture)) {
		LogManager::GetInstance().Log("Failed to create FMOD sprite!");
		delete m_fmodSprite;
		m_fmodSprite = nullptr;
		delete m_fmodTexture;
		return false;
	}
	m_sprites.push_back(m_fmodSprite);

	m_isAssetsLoaded = true;

	m_state = SplashState::Waiting;
	m_timer = 0.0f;
	m_alpha = 0.0f;

	return true;
}

void
SceneSplash::Process(float deltaTime, InputSystem& inputSystem)
{
	if (!m_isAssetsLoaded) {
		// Don't start fade in until assets are loaded
		return;
	}

	// Check for input
	if (inputSystem.GetKeyState(SDL_SCANCODE_RETURN) == BS_PRESSED ||
		inputSystem.GetController(0) && inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED)
	{
		Game::GetInstance().SetCurrentScene(1); // Skip Splash
	}

	// Update the timer
	m_timer += deltaTime;

	switch (m_state) {
	case SplashState::Waiting:
		if (m_timer >= 1.0f) {
			m_state = SplashState::FadeInAUT;
			m_timer = 0.0f;
		}
		break;

	case SplashState::FadeInAUT:
		m_alpha = std::min(1.0f, m_timer / FADE_DURATION);
		if (m_timer >= FADE_DURATION) {
			m_state = SplashState::ShowAUT; // Switch to next state
			m_timer = 0.0f;
		}
		break;

	case SplashState::ShowAUT:
		m_alpha = 1.0f;
		if (m_timer >= SHOW_DURATION) {
			m_state = SplashState::FadeOutAUT; 
			m_timer = 0.0f;
		}
		break;

	case SplashState::FadeOutAUT:
		m_alpha = 1.0f - std::min(1.0f, m_timer / FADE_DURATION);
		if (m_timer >= FADE_DURATION) {
			m_state = SplashState::FadeInFMOD; 
			m_timer = 0.0f;
		}
		break;

	case SplashState::FadeInFMOD:
		m_alpha = std::min(1.0f, m_timer / FADE_DURATION);
		if (m_timer >= FADE_DURATION) {
			m_state = SplashState::ShowFMOD;
			m_timer = 0.0f;
		}
		break;

	case SplashState::ShowFMOD:
		m_alpha = 1.0f;
		if (m_timer >= SHOW_DURATION) {
			m_state = SplashState::FadeOutFMOD;
			m_timer = 0.0f;
		}
		break;
	case SplashState::FadeOutFMOD:
		m_alpha = 1.0f - std::min(1.0f, m_timer / FADE_DURATION);
		if (m_timer >= FADE_DURATION) {
			m_state = SplashState::Done;
			m_timer = 0.0f;
		}
		break;

	case SplashState::Done:
		m_alpha = 0.0f;
		break;
	}

	if (m_state == SplashState::Done) {
		Game::GetInstance().SetCurrentScene(1); 
	}
}

void
SceneSplash::Draw(Renderer& renderer)
{
	// Draw the background
	float scaleX = static_cast<float>(m_screenWidth) / m_backgroundTexture->GetWidth();
	float scaleY = static_cast<float>(m_screenHeight) / m_backgroundTexture->GetHeight();
	float scale = std::max(scaleX, scaleY);
	m_backgroundSprite->SetScale(scale);

	m_backgroundSprite->SetX(m_screenWidth / 2);
	m_backgroundSprite->SetY(m_screenHeight / 2);

	m_backgroundSprite->Draw(renderer);

	if (m_isAssetsLoaded) {
		DrawLogos();
	}
}

void
SceneSplash::DrawLogos() {
	if (m_state == SplashState::FadeInAUT || m_state == SplashState::ShowAUT || m_state == SplashState::FadeOutAUT) {
		m_autSprite->SetAlpha(m_alpha);
		m_autSprite->SetX(m_screenWidth / 2);
		m_autSprite->SetY(m_screenHeight / 2);
		m_autSprite->SetScale(1.5f);
		m_autSprite->Draw(*m_pRenderer);
	}
	else if (m_state == SplashState::FadeInFMOD || m_state == SplashState::ShowFMOD || m_state == SplashState::FadeOutFMOD) {
		m_fmodSprite->SetAlpha(m_alpha);
		m_fmodSprite->SetX(m_screenWidth / 2);
		m_fmodSprite->SetY(m_screenHeight / 2);
		m_fmodSprite->Draw(*m_pRenderer);
	}
}

void
SceneSplash::DebugDraw
()
{
	ImGui::Text("Scene: Splash");
}