// This include:
#include "scenemain.h"

// Local includes:
#include "renderer.h"
#include "sprite.h"
#include "imgui/imgui.h"
#include "inputsystem.h"
#include "game.h"
#include "logmanager.h"
#include "player.h"


// Library includes:
#include <cassert>
#include "fmod.hpp"

SceneMain::SceneMain()
{
}

SceneMain::~SceneMain() 
{

}

bool
SceneMain::Initialise(Renderer& renderer)
{
	// Load Player Texture
	static Texture* playerTexture = new Texture();
	playerTexture->Initialise("../assets/ball.png"); // Replace later
	m_player.Initialise(renderer, *playerTexture);

	return true;
}

void
SceneMain::Process(float deltaTime, InputSystem& inputSystem)
{
	m_player.Process(deltaTime, inputSystem);
}

void
SceneMain::Draw(Renderer& renderer)
{
	m_player.Draw(renderer);
}

void SceneMain::DebugDraw
()
{
	ImGui::Text("Scene: Main");
}