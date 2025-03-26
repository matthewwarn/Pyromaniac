// COMP710 GP Framework 2022
// This include:
#include "game.h"

// Library includes:
#include "renderer.h"
#include "logmanager.h"
#include "sprite.h"
#include <iostream>

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

bool Game::Initialise()
{
	int bbWidth = 960;
	int bbHeight = 720;

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

	srand(static_cast<unsigned int>(time(0)));

	InitBalls();

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

	for (auto& ball : m_balls) {
		ball.x += ball.vx * deltaTime * 200;
		ball.y += ball.vy * deltaTime * 200;

		float halfSize = (ball.sprite->GetWidth() * ball.scale) / 2.0f;
		
		int screenWidth = m_pRenderer->GetWidth();
		int screenHeight = m_pRenderer->GetHeight();

		if (ball.x - halfSize < 0) {
			ball.x = halfSize; 
			ball.vx = -ball.vx; 
		}
		
		if (ball.x + halfSize > screenWidth) {
			ball.x = screenWidth - halfSize;
			ball.vx = -ball.vx;
		}
		
		if (ball.y - halfSize < 0) {
			ball.y = halfSize;
			ball.vy = -ball.vy;
		}

		if (ball.y + halfSize > screenHeight) {
			ball.y = screenHeight - halfSize;
			ball.vy = -ball.vy;
		}
		
		ball.sprite->SetX(ball.x);
		ball.sprite->SetY(ball.y);
	}
}

void Game::Draw(Renderer& renderer)
{

	++m_iFrameCount;

	renderer.Clear();

	// Draw balls
	for (auto& ball : m_balls) {
		ball.sprite->Draw(renderer);
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

void Game::InitBalls()
{
	for (int i = 0; i < 100; i++) {
		// Load sprite
		Sprite* ballSprite = m_pRenderer->CreateSprite("../assets/ball.png");

		if (!ballSprite) {
			LogManager::GetInstance().Log("Failed to load ball sprite!");
		}

		if (ballSprite) {

			Ball ball;
			ball.sprite = ballSprite;
			ball.x = m_pRenderer->GetWidth() / 2.0f;
			ball.y = m_pRenderer->GetHeight() / 2.0f;

			// Random velocity
			ball.vx = ((rand() % 200) - 100) / 100.0f;
			ball.vy = ((rand() % 200) - 100) / 100.0f;

			// Random scale
			ball.scale = (rand() % 50) / 100.0f * 0.5f;
			ball.sprite->SetScale(ball.scale);

			// Random colour
			ball.r = (rand() % 256) / 255.0f;
			ball.g = (rand() % 256) / 255.0f;
			ball.b = (rand() % 256) / 255.0f;
			ball.sprite->SetRedTint(ball.r);
			ball.sprite->SetGreenTint(ball.g);
			ball.sprite->SetBlueTint(ball.b);

			ball.sprite->SetX(ball.x);
			ball.sprite->SetY(ball.y);

			m_balls.push_back(ball);

			std::cout << "SPRITE LOADED" << std::endl;
			std::cout << "Screen: " << m_pRenderer->GetWidth() << " x " << m_pRenderer->GetHeight() << std::endl;
			std::cout << "Ball initial position: (" << ball.x << ", " << ball.y << ")" << std::endl;
			std::cout << "Ball velocity: (" << ball.vx << ", " << ball.vy << ")" << std::endl;

		}
	}
}