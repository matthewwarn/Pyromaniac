#ifndef POWERUPGENOCIDE_H
#define POWERUPGENOCIDE_H

#include "powerup.h"

class SceneMain;

class PowerupGenocide : public Powerup {
public:
	PowerupGenocide(const Vector2& position);

	void ApplyPowerup(Player& player, SceneMain& scene) override;

private:
	int m_points = 5000;

};
#endif // POWERUPGENOCIDE_H