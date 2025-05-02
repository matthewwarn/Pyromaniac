#ifndef PLAYER_H
#define PLAYER_H

#include "vector2.h"
#include "sprite.h"

class Renderer;
class Texture;
class InputSystem;

class Player
{
	// Member methods:
public:
	Player();
	~Player();
	bool Initialise(Renderer& renderer, Texture& texture);
	void Process(float deltaTime, InputSystem& inputSystem);
	void Draw(Renderer& renderer);
	void Movement(float deltaTime, InputSystem& inputSystem);
	Vector2& GetPosition();

private:
	Sprite m_sprite;
	Vector2 m_position;
	float m_speed;
};

#endif // PLAYER_H