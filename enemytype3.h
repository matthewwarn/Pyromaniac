#ifndef ENEMYTYPE3_H
#define ENEMYTYPE3_H

#include "enemy.h"

// ENEMY 3: Slow and Tanky

class EnemyType3 : public Enemy
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

		m_animatedSprite->SetupFrames(25, 28);
		m_animatedSprite->SetFrameDuration(0.2f);
		m_animatedSprite->SetLooping(true);
		m_animatedSprite->Animate();
		m_animatedSprite->SetScale(5.0f);

		m_sprite = m_animatedSprite;

		m_originalSpeed = 40.0f;
		m_speed = 40.0f;
		m_health = 250.0f;

		return true;
	}

	int GetRadius() override
	{
		return (m_animatedSprite->GetFrameWidth() / 2) * m_animatedSprite->GetScale() * 0.8f;
	}
};

#endif // ENEMYTYPE3_H