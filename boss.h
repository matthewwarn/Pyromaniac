#ifndef BOSS_H
#define BOSS_H

#include "enemy.h"

// BOSS: Fat and Dangerous

class Boss : public Enemy
{
	// Member methods:
public:
	bool Initialise(Renderer& renderer, const char* spritePath, int screenWidth, int screenHeight) override {

		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;

		m_animatedSprite = renderer.CreateAnimatedSprite(spritePath);

		if (!m_animatedSprite) {
			return false;
		}

		m_animatedSprite->SetupFrames(45, 42);
		m_animatedSprite->SetFrameDuration(0.1f);
		m_animatedSprite->SetLooping(true);
		m_animatedSprite->Animate();
		m_animatedSprite->SetScale(12.0f);

		m_sprite = m_animatedSprite;

		m_originalSpeed = 55.0f;
		m_speed = 55.0f;
		m_health = 3000.0f;

		return true;
	}

	int GetRadius() override
	{
		return (m_animatedSprite->GetFrameWidth() / 2) * m_animatedSprite->GetScale() * 0.5f;
	}
};

#endif // BOSS_H