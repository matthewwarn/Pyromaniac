// COMP710 GP Framework 2022
// This include:
#include "game.h"

// Library includes:
#include "renderer.h"
#include "logmanager.h"
#include "sprite.h"

// Static Members:
Game* Game::sm_pInstance = 0;

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

Game::Game() : m_pRenderer(0), m_pCheckerboard(0), m_bLooping(true)
{

}

Game::~Game()
{
	delete m_pCheckerboard;
	m_pCheckerboard = 0;

	delete m_pRenderer;
	m_pRenderer = 0;
}

void Game::Quit()
{
	m_bLooping = false;
}

// ONLY ON 5x02 BRANCH

bool Game::Initialise()
{
	int bbWidth = 1024;
	int bbHeight = 768;

	m_pRenderer = new Renderer();
	if (!m_pRenderer->Initialise(true, bbWidth, bbHeight))
	{
		LogManager::GetInstance().Log("Renderer failed to initialise!");
		return false;
	}

	bbWidth = m_pRenderer->GetWidth();
	bbHeight = m_pRenderer->GetHeight();

	m_iLastTime = SDL_GetPerformanceCounter();

	m_pRenderer->SetClearColour(0, 255, 255);

	// Checkerboard
	m_pCheckerboard = m_pRenderer->CreateSprite("../assets/board8x8.png");
	if (m_pCheckerboard) {
		m_pCheckerboard->SetX(bbWidth / 2.0f);
		m_pCheckerboard->SetY(bbHeight / 2.0f);
	}

	return true;
}

bool Game::DoGameLoop()
{
	const float stepSize = 1.0f / 60.0f;

	// TODO: Process input here!
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		continue;
	}

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

	// TODO: Add game objects to process here!
	if (m_pCheckerboard) {
		m_pCheckerboard->Process(deltaTime);
	}
}

void Game::Draw(Renderer& renderer)
{

	++m_iFrameCount;

	renderer.Clear();

	// Draw checkerboard
	if (m_pCheckerboard) {
		m_pCheckerboard->Draw(renderer);
	}

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