// COMP710 GP Framework 2022
// This include:
#include "game.h"

// Library includes:
#include "renderer.h"
#include "logmanager.h"
#include "sprite.h"
#include <iostream>
#include "imgui/imgui_impl_sdl2.h"
#include "inputsystem.h"
#include "xboxcontroller.h"
#include "fmod.hpp"

// Scene includes:
#include "scenesplash.h"
#include "scenemenu.h"
#include "scenemain.h"
#include "scenecheckerboards.h"
#include "scenebouncingballs.h"

// Static Members:
Game* Game::sm_pInstance = 0;
FMOD::System* m_pFMODSystem = nullptr;

Game& Game::GetInstance()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new Game();
	}

	return (*sm_pInstance);
}

void Game::DestroyInstance()
{
	delete sm_pInstance;
	sm_pInstance = 0;
}

Game::Game() : m_pRenderer(0), m_bLooping(true)
{

}

Game::~Game()
{
	if (m_pFMODSystem)
	{
		m_pFMODSystem->close();
		m_pFMODSystem->release();
		m_pFMODSystem = 0;
	}

	delete m_pRenderer;
	m_pRenderer = 0;

	delete m_pInputSystem;
	m_pInputSystem = 0;

	for (Scene* scene : m_scenes){
		delete scene;
	}
	m_scenes.clear();
}

void Game::Quit()
{
	m_bLooping = false;
}

bool Game::Initialise()
{
	int bbWidth = 720;
	int bbHeight = 480;

	m_pRenderer = new Renderer();
	if (!m_pRenderer->Initialise(false, bbWidth, bbHeight))
	{
		LogManager::GetInstance().Log("Renderer failed to initialise!");
		return false;
	}

	bbWidth = m_pRenderer->GetWidth();
	bbHeight = m_pRenderer->GetHeight();

	m_iLastTime = SDL_GetPerformanceCounter();

	m_pRenderer->SetClearColour(0, 255, 255);

	// Initialise FMOD
	FMOD::System_Create(&m_pFMODSystem);
	m_pFMODSystem->init(512, FMOD_INIT_NORMAL, 0);

	// Initialise Input System
	if (m_pInputSystem != nullptr)
	{
		delete m_pInputSystem;
		m_pInputSystem = nullptr;
	}

	m_pInputSystem = new InputSystem();
	
	if (!m_pInputSystem->Initialise())
	{
		LogManager::GetInstance().Log("InputSystem failed to initialise!");
		return false;
	}

	// Generate random seed
	srand(static_cast<unsigned int>(time(0)));

	// Initialise Scenes
	Scene* pScene = 0;

	pScene = new SceneSplash();
	if (!pScene->Initialise(*m_pRenderer))
	{
		delete pScene;
		LogManager::GetInstance().Log("SceneSplash failed to initialise!");
		return false;
	}
	m_scenes.push_back(pScene);

	pScene = new SceneMenu();
	if (!pScene->Initialise(*m_pRenderer))
	{
		delete pScene;
		LogManager::GetInstance().Log("SceneMenu failed to initialise!");
		return false;
	}
	m_scenes.push_back(pScene);

	pScene = new SceneMain();
	if (!pScene->Initialise(*m_pRenderer))
	{
		delete pScene;
		LogManager::GetInstance().Log("SceneMain failed to initialise!");
		return false;
	}
	m_scenes.push_back(pScene);

	//pScene = new SceneCheckerboards(m_pFMODSystem);
	//if (!pScene->Initialise(*m_pRenderer))
	//{
	//	delete pScene;
	//	LogManager::GetInstance().Log("SceneCheckerboards failed to initialise!");
	//	return false;
	//}
	//m_scenes.push_back(pScene);

	//pScene = new SceneBouncingBalls();
	//if (!pScene->Initialise(*m_pRenderer))
	//{
	//	delete pScene;
	//	LogManager::GetInstance().Log("SceneBouncingBalls failed to initialise!");
	//	return false;
	//}
	//m_scenes.push_back(pScene);

	m_iCurrentScene = 0;

	return true;
}

bool Game::DoGameLoop()
{
	const float stepSize = 1.0f / 60.0f;

	// Process input 
	m_pInputSystem->ProcessInput();

	if (m_bLooping)
	{
		Uint64 current = SDL_GetPerformanceCounter();
		float deltaTime = (current - m_iLastTime) / static_cast<float>(SDL_GetPerformanceFrequency());

		m_iLastTime = current;

		m_fExecutionTime += deltaTime;

		Process(deltaTime);

#ifdef USE_LAG
		m_fLag += deltaTime;

		int innerLag = 0;

		while (m_fLag >= stepSize)
		{
			Process(stepSize);

			m_fLag -= stepSize;

			++m_iUpdateCount;
			++innerLag;
		}
#endif //USE_LAG

		Draw(*m_pRenderer);
	}

	return m_bLooping;
}

void Game::Process(float deltaTime)
{
	ProcessFrameCounting(deltaTime);

	m_scenes[m_iCurrentScene]->Process(deltaTime, *m_pInputSystem);

	// Toggle Debug Window
	ButtonState backspaceState = m_pInputSystem->GetKeyState(SDL_SCANCODE_BACKSPACE);
	if (backspaceState == BS_PRESSED)
	{
		ToggleDebugWindow();
	}
}

void Game::Draw(Renderer& renderer)
{
	++m_iFrameCount;

	renderer.Clear();

	// Draw Current Scene
	m_scenes[m_iCurrentScene]->Draw(renderer);

	DebugDraw();

	renderer.Present();
}

void
Game::ProcessFrameCounting(float deltaTime)
{
	// Count total simulation time elapsed:
	m_fElapsedSeconds += deltaTime;

	// Frame Counter:
	if (m_fElapsedSeconds > 1.0f)
	{
		m_fElapsedSeconds -= 1.0f;
		m_iFPS = m_iFrameCount;
		m_iFrameCount = 0;
	}
}

void Game::DebugDraw
()
{
	if (m_bShowDebugWindow) {
		bool open = true;

		ImGui::Begin("Debug Window", &open, ImGuiWindowFlags_MenuBar);

		ImGui::Text("COMP710 GP Framework (%s)", "2025, S1");

		if (ImGui::Button("Quit"))
		{
			Quit();
		}

		ImGui::SliderInt("Active scene", &m_iCurrentScene, 0, m_scenes.size() - 1, "%d");

		m_scenes[m_iCurrentScene]->DebugDraw();

		ImGui::End();
	}
}

void Game::ToggleDebugWindow
()
{
	m_bShowDebugWindow = !m_bShowDebugWindow;
	m_pInputSystem->ShowMouseCursor(m_bShowDebugWindow);
}

void Game::SetCurrentScene(int index)
{
	if (index >= 0 && index < static_cast<int>(m_scenes.size()))
	{
		m_iCurrentScene = index;
	}
}