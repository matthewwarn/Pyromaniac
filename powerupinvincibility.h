#ifndef POWERUPINVINCIBILITY_H
#define POWERUPINVINCIBILITY_H

#include "powerup.h"
class SceneMain;

class PowerupInvincibility : public Powerup {
public:
	PowerupInvincibility(const Vector2& position)
		: Powerup(position)
	{
		static Texture* invincibleTexture = new Texture();
		invincibleTexture->Initialise("../assets/ball.png"); // Replace later
		Initialise(*invincibleTexture);

		m_sprite.SetRedTint(0.5f);
		m_sprite.SetBlueTint(0.5f);
		m_sprite.SetGreenTint(0.0f);

		m_position = position;
	}

	void ApplyPowerup(Player& player, SceneMain& scene) override {
		player.SetInvincible();
	}
};

#endif // POWERUPINVINCIBILITY_H