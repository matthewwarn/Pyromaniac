#ifndef POWERUP_H
#define POWERUP_H

#include "sprite.h"
#include "renderer.h"
#include "player.h"

// Forward Declarations:
class SceneMain;

class Powerup {
// Member Methods:
public:
	Powerup(const Vector2& position)
		: m_position(position) 
	{
	}
	
	virtual ~Powerup() = default;

	virtual bool Initialise(Texture& texture)
	{
		m_sprite.Initialise(texture);
		m_sprite.SetScale(2.5f);
		SetPosition(m_position);
		return true;
	}

	virtual void Process(float deltaTime) 
	{
		m_lifetime += deltaTime;
	}

	virtual void Draw(Renderer& renderer) {
		m_sprite.Draw(renderer);
	}

	// When player collects. Override in subclass.
	virtual void ApplyPowerup(Player& player, SceneMain& scene) = 0;

	const Vector2& GetPosition() const {
		return m_position;
	}

	void SetPosition(const Vector2& position) {
		m_position = position;
		m_sprite.SetX(static_cast<int>(position.x));
		m_sprite.SetY(static_cast<int>(position.y));
	}

	float GetRadius() const {
		return m_sprite.GetWidth() / 2.0f;
	}

	bool IsExpired() const {
		return m_lifetime > 15.0f;  // Return true if the powerup has been on screen for more than 15 seconds
	}

// Member Data:
protected:
	Sprite m_sprite;
	Vector2 m_position;
	float m_radius = m_sprite.GetWidth() / 2.0f;
	float m_lifetime;
};

#endif // POWERUP_H