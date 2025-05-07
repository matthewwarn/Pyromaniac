#include "powerupgenocide.h"
#include "scenemain.h"

PowerupGenocide::PowerupGenocide(const Vector2& position)
    : Powerup(position)
{
    static Texture* genocideTexture = new Texture();
    genocideTexture->Initialise("../assets/ball.png"); // Replace later
    Initialise(*genocideTexture);

    m_sprite.SetRedTint(0.0f);
    m_sprite.SetBlueTint(0.0f);
    m_sprite.SetGreenTint(0.0f);

    m_position = position;
}

void PowerupGenocide::ApplyPowerup(Player& player, SceneMain& scene)
{
    scene.KillAllEnemies(); //  Now SceneMain is fully defined
}