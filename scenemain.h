#ifndef SCENEMAIN_H
#define SCENEMAIN_H

// Local includes:
#include "scene.h"
#include "animatedsprite.h"
#include "fmod.hpp"
#include "player.h"

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

protected:
	Player m_player;
private:
	SceneMain(const SceneMain& sceneMain) = delete;
	SceneMain& operator=(const SceneMain& sceneMain) = delete;

	// Member data:
public:

protected:

private:

};

#endif // SCENEMAIN_H