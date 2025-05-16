#ifndef ENEMYTYPE2_H
#define ENEMYTYPE2_H

#include "enemy.h"

// ENEMY 2: Fast and Weak

class EnemyType2 : public Enemy
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

		m_animatedSprite->SetupFrames(64, 64);
		m_animatedSprite->SetFrameDuration(0.1f);
		m_animatedSprite->SetLooping(true);
		m_animatedSprite->Animate();
		m_animatedSprite->SetScale(2.5f);

		m_sprite = m_animatedSprite;

		m_originalSpeed = 125.0f;
		m_speed = 125.0f;
		m_health = 20.0f;

		m_points = 1500;

		return true;
	}

	int GetRadius() override
	{
		return (m_animatedSprite->GetFrameWidth() / 2) * m_animatedSprite->GetScale() * 0.3f;
	}
};

#endif // ENEMYTYPE2_H