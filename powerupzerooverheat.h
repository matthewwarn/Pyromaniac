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
		zeroOverheatTexture->Initialise("../assets/potion2.png"); 
		Initialise(*zeroOverheatTexture);

		m_position = position;
	}

	void ApplyPowerup(Player& player, SceneMain& scene) override {
		player.SetZeroOverheat();
	}
};

#endif // POWERUPZEROOVERHEAT_H