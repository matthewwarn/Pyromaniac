#ifndef POWERUPZEROOVERHEAT_H
#define POWERUPZEROOVERHEAT_H

#include "powerup.h"
class SceneMain;

class PowerupZeroOverheat : public Powerup {
public:
	PowerupZeroOverheat(const Vector2& position)
		: Powerup(position)
	{
		static Texture* zeroOverheatTexture = new Texture();
		zeroOverheatTexture->Initialise("../assets/ball.png"); // Replace later
		Initialise(*zeroOverheatTexture);
		
		m_sprite.SetRedTint(0.741f);
		m_sprite.SetBlueTint(0.925f);
		m_sprite.SetGreenTint(0.871f);

		m_position = position;
	}

	void ApplyPowerup(Player& player, SceneMain& scene) override {
		player.SetZeroOverheat();
	}
};

#endif // POWERUPZEROOVERHEAT_H