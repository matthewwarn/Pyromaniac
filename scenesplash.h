#ifndef SCENESPLASH_H
#define SCENESPLASH_H

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
class SceneSplash : public Scene
{
	// Member methods:
public:
	SceneSplash();
	virtual ~SceneSplash();

	virtual bool Initialise(Renderer& renderer);
	virtual void Draw(Renderer& renderer);
	virtual void DebugDraw();
	virtual void Process(float deltaTime, InputSystem& inputSystem);

	void DrawLogos();

protected:

private:

	// Member Data:
public:
	enum class SplashState {
		Waiting,
		FadeInFMOD,
		ShowFMOD,
		FadeOutFMOD,
		Done
	};

protected:
	Renderer* m_pRenderer;
	int m_screenWidth;
	int m_screenHeight;

	SplashState m_state;

	float m_alpha = 0.0f;
	float m_timer = 0.0f;
	const float FADE_DURATION = 1.7f; 
	const float SHOW_DURATION = 1.3f;   

	bool m_isAssetsLoaded = false;

	std::vector<Sprite*> m_sprites;
	Sprite* m_backgroundSprite;
	Sprite* m_fmodSprite;

private:

};

#endif // SCENESPLASH_H