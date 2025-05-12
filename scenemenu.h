#ifndef SCENEMENU_H
#define SCENEMENU_H

// Local includes:
#include "scene.h"
#include "texture.h"

// Library includes:
#include <vector>
#include <memory>

// Forward declarations:
class Renderer;
class Sprite;

// Class declaration:
class SceneMenu : public Scene
{
	// Member methods:
public:
	SceneMenu();
	virtual ~SceneMenu();

	virtual bool Initialise(Renderer& renderer);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();
	virtual void Process(float deltaTime, InputSystem& inputSystem);

protected:

private:

	// Member Data:
public:
	
protected:
	Renderer* m_pRenderer;
	int m_screenWidth;
	int m_screenHeight;
	
	Texture* m_backgroundTexture = nullptr;
	Sprite* m_backgroundSprite;

private:

};

#endif // SCENEMENU_H