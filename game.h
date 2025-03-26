// COMP710 GP Framework
#include <vector>
#ifndef GAME_H
#define GAME_H

// Forward declarations:
class Renderer;
class Sprite;

class Game
{
	// Member methods:
public:
	static Game& GetInstance();
	static void DestroyInstance();

	bool Initialise();
	bool DoGameLoop();
	void Quit();
	void InitBalls();

protected:
	void Process(float deltaTime);
	void Draw(Renderer& renderer);

	void ProcessFrameCounting(float deltaTime);

private:
	Game();
	~Game();
	Game(const Game& game);
	Game& operator=(const Game& game);


	// Member data:
public:

	// Ball struct
	struct Ball
	{
		Sprite* sprite;
		float x;
		float y;
		float vx;
		float vy;
		float scale;
		float r, g, b;
	};

	std::vector<Ball> m_balls;

	float bbWidth;
	float bbHeight;

protected:
	static Game* sm_pInstance;
	Renderer* m_pRenderer;
	Sprite* m_pCheckerboard;

	__int64 m_iLastTime;
	float m_fExecutionTime;
	float m_fElapsedSeconds;
	int m_iFrameCount;
	int m_iFPS;

#ifdef USE_LAG
	float m_fLag;
	int m_iUpdateCount;
#endif // USE_LAG

	bool m_bLooping;

private:

};

#endif // GAME_H