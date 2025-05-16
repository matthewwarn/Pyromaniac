#ifndef ENEMYTYPE1_H
#define ENEMYTYPE1_H

#include "enemy.h"

// ENEMY 1: Sturdy and Steady

class EnemyType1 : public Enemy
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

		m_animatedSprite->SetupFrames(16, 16);
		m_animatedSprite->SetFrameDuration(0.1f);
		m_animatedSprite->SetLooping(true);
		m_animatedSprite->Animate();
		m_animatedSprite->SetScale(4.5f);

		m_sprite = m_animatedSprite;
		
		m_originalSpeed = 60.0f;
		m_speed = 60.0f;
		m_health = 80.0f;

		m_points = 1000;

		return true;
	}

	int GetRadius() override
	{
		return (m_animatedSprite->GetFrameWidth() / 2) * m_animatedSprite->GetScale() * 0.9f;
	}
};

#endif // ENEMYTYPE1_H