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
		invincibleTexture->Initialise("../assets/potion1.png"); 
		Initialise(*invincibleTexture);

		m_position = position;
	}

	void ApplyPowerup(Player& player, SceneMain& scene) override {
		player.SetInvincible();
	}
};

#endif // POWERUPINVINCIBILITY_H