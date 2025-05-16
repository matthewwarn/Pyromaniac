#include "powerupgenocide.h"
#include "scenemain.h"

PowerupGenocide::PowerupGenocide(const Vector2& position)
    : Powerup(position)
{
    static Texture* genocideTexture = new Texture();
    genocideTexture->Initialise("../assets/potion3.png");
    Initialise(*genocideTexture);

    m_position = position;
}

void PowerupGenocide::ApplyPowerup(Player& player, SceneMain& scene)
{
    scene.m_skipScore = true;
    scene.KillAllEnemies(); 
}