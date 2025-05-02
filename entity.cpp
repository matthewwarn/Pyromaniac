#include "entity.h"
#include "sprite.h"
#include "renderer.h"
#include "vector2.h"

Entity::Entity()
	: m_pSprite(nullptr)
	, m_position(0.0f, 0.0f)
	, m_velocity(0.0f, 0.0f)
	, m_bAlive(true)
{
}

Entity::~Entity()
{
	// Clean up the sprite if it exists
	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = nullptr;
	}
}

bool Entity::Initialise(Renderer& renderer)
{
	Texture* texture = new Texture();

	// If fails to load texture
	if (!texture->Initialise("../assets/ball.png"))
	{
		delete texture;
		return false;  
	}

	m_pSprite = new Sprite();

	// If fails to load sprite
	if (!m_pSprite->Initialise(*texture))  
	{
		delete m_pSprite;
		m_pSprite = nullptr;
		delete texture;
		return false;  
	}

	// Set initial position for the sprite
	m_pSprite->SetX(static_cast<int>(m_position.x));
	m_pSprite->SetY(static_cast<int>(m_position.y));

	// Clean up texture 
	delete texture;

	return true;  
}


void Entity::Process(float deltaTime)
{
	// Update position based on velocity
	m_position += m_velocity * deltaTime;

	// Update sprite position
	if (m_pSprite)
	{
		m_pSprite->SetX(static_cast<int>(m_position.x));
		m_pSprite->SetY(static_cast<int>(m_position.y));
	}
}

void Entity::Draw(Renderer& renderer)
{
	// Only draw if sprite is valid and the entity is alive
	if (m_pSprite && m_bAlive)
	{
		// Draw the sprite
		m_pSprite->Draw(renderer);
	}
}

void Entity::Rotate(float direction)
{
	// Rotate the entity by the given angle (in degrees or radians)
	float newAngle = m_pSprite->GetAngle() + direction;
	m_pSprite->SetAngle(newAngle);
}

bool Entity::IsAlive() const
{
	return m_bAlive;
}

void Entity::SetDead()
{
	m_bAlive = false;
}

float Entity::GetRadius()
{
	// Return the radius for collision (based on the sprite's size)
	return static_cast<float>(m_pSprite->GetWidth() / 2);
}

Vector2 Entity::GetFacingDirection()
{
	// Return a direction based on the sprite's rotation
	float angleInRadians = m_pSprite->GetAngle() * (3.14159f / 180.0f); 
	return Vector2(cos(angleInRadians), sin(angleInRadians)); 
}

Vector2& Entity::GetPosition()
{
	return m_position;
}

Vector2& Entity::GetVelocity()
{
	return m_velocity;
}

bool Entity::IsCollidingWith(Entity& toCheck)
{
	Vector2 diff = m_position - toCheck.GetPosition();
	float distanceSq = diff.LengthSquared();
	float combinedRadius = this->GetRadius() + toCheck.GetRadius();
	return distanceSq <= (combinedRadius * combinedRadius);
}