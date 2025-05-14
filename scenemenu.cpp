// This include:
#include "scenemenu.h"

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

SceneMenu::SceneMenu()
{
}

SceneMenu::~SceneMenu()
{
	if (m_backgroundTexture) {
		delete m_backgroundTexture;
		m_backgroundTexture = nullptr;
	}

	if (m_backgroundSprite) {
		delete m_backgroundSprite;
		m_backgroundSprite = nullptr;
	}
}

bool
SceneMenu::Initialise(Renderer& renderer)
{
	m_pRenderer = &renderer;
	m_screenWidth = renderer.GetWidth();
	m_screenHeight = renderer.GetHeight();

	// Load the background texture
	Texture* backgroundTexture = new Texture();
	if (!backgroundTexture->Initialise("../assets/main_menu.png")) {
		LogManager::GetInstance().Log("Failed to load menu background texture!");
		delete backgroundTexture;
		return false;
	}

	m_backgroundTexture = backgroundTexture;

	m_backgroundSprite = new Sprite();
	if (!m_backgroundSprite->Initialise(*backgroundTexture)) {
		LogManager::GetInstance().Log("Failed to create menu background sprite!");
		delete m_backgroundSprite;
		m_backgroundSprite = nullptr;
		delete backgroundTexture;
		return false;
	}

	return true;
}

void
SceneMenu::Process(float deltaTime, InputSystem& inputSystem)
{
	// Check for inputs
	if (inputSystem.GetKeyState(SDL_SCANCODE_RETURN) == BS_PRESSED ||
		inputSystem.GetController(0) && inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_START) == BS_PRESSED)
	{
		Game::GetInstance().SetCurrentScene(2); // Start the game
	}

	if (inputSystem.GetKeyState(SDL_SCANCODE_ESCAPE) == BS_PRESSED ||
		inputSystem.GetController(0) && inputSystem.GetController(0)->GetButtonState(SDL_CONTROLLER_BUTTON_BACK) == BS_PRESSED)
	{
		Game::GetInstance().Quit(); // Quit the game
	}
}

void
SceneMenu::Draw(Renderer& renderer)
{
	// Draw the background
	float scaleX = static_cast<float>(m_screenWidth) / m_backgroundTexture->GetWidth();
	float scaleY = static_cast<float>(m_screenHeight) / m_backgroundTexture->GetHeight();
	float scale = std::max(scaleX, scaleY);
	m_backgroundSprite->SetScale(scale);

	m_backgroundSprite->SetX(m_screenWidth / 2);
	m_backgroundSprite->SetY(m_screenHeight / 2);

	m_backgroundSprite->Draw(renderer);
}

void
SceneMenu::DebugDraw
()
{
	ImGui::Text("Scene: Menu");
}