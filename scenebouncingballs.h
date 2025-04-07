// COMP710 GP Framework
#ifndef SCENEBOUNCINGBALLS_H
#define SCENEBOUNCINGBALLS_H

// Local includes:
#include "scene.h"

// Forward declarations:
class Renderer;
class Ball;
class Sprite;

// Class declaration:
class SceneBouncingBalls : public Scene
{
	// Member methods:
public:
	SceneBouncingBalls();
	virtual ~SceneBouncingBalls();

	virtual bool Initialise(Renderer& renderer);
	virtual void Process(float deltaTime, InputSystem& inputSystem);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();

protected:

private:
	SceneBouncingBalls(const SceneBouncingBalls& sceneBouncingBalls);
	SceneBouncingBalls& operator=(const SceneBouncingBalls& sceneBouncingBalls);
	
// Member data:
public:

protected:
	Ball* m_pBalls[100];

	int m_iShowCount;

	Sprite* m_pWelcomeText;

private:

};

#endif // SCENEBOUNCINGBALLS_H