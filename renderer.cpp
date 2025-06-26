
// This include:
#include "renderer.h"

// Local includes:
#include "texturemanager.h"
#include "logmanager.h"
#include "shader.h"
#include "vertexarray.h"
#include "sprite.h"
#include "matrix4.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "animatedsprite.h"
#include "texture.h"

// Library includes:
#include <SDL.h>
#include <SDL_image.h>
#include <glew.h>
#include <cassert>
#include <cmath>

Renderer::Renderer()
	: m_pTextureManager(0)
	, m_pSpriteShader(0)
	, m_pSpriteVertexData(0)
	, m_glContext(0)
	, m_iWidth(0)
	, m_iHeight(0)
	, m_fClearRed(0.0f)
	, m_fClearGreen(0.0f)
	, m_fClearBlue(0.0f)
	, m_pWhiteTexture(nullptr)
	, m_pWindow(nullptr)
{

}

Renderer::~Renderer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	delete m_pSpriteShader;
	m_pSpriteShader = 0;

	delete m_pSpriteVertexData;
	m_pSpriteVertexData = 0;

	delete m_pWhiteTexture;
	m_pWhiteTexture = 0;

	delete m_pTextureManager;
	m_pTextureManager = 0;

	//SDL_Destroy
	
	(m_pWindow);
	IMG_Quit();
	SDL_Quit();
}

bool Renderer::Initialise(bool windowed, int width, int height)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LogSdlError();
		return false;
	}

	m_fullscreen = false;

	if (!windowed)
	{
		// Go fullscreen, with current resolution!
		int numDisplays = SDL_GetNumVideoDisplays();
		SDL_DisplayMode* currentDisplayMode = new SDL_DisplayMode[numDisplays];

		for (int k = 0; k < numDisplays; ++k)
		{
			int result = SDL_GetCurrentDisplayMode(k, &currentDisplayMode[k]);
		}

		// Use the widest display?
		int widest = 0;
		int andItsHeight = 0;

		for (int k = 0; k < numDisplays; ++k)
		{
			if (currentDisplayMode[k].w > widest)
			{
				widest = currentDisplayMode[k].w;
				andItsHeight = currentDisplayMode[k].h;
			}
		}

		delete[] currentDisplayMode;
		currentDisplayMode = 0;

		width = widest;
		height = andItsHeight;

		m_fullscreen = true;
	}

	bool initialised = InitialiseOpenGL(width, height);

	if (initialised)
	{
		m_pTextureManager = new TextureManager();
		assert(m_pTextureManager);
		initialised = m_pTextureManager->Initialise();
	}

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(m_pWindow, m_glContext);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	// Baking a version of the text at different sizes to prevent blur
	ImGuiIO& io = ImGui::GetIO();
	m_bigFont = io.Fonts->AddFontFromFileTTF("joystix_monospace.otf", 64.0f);
	if (!m_bigFont)
	{
		LogManager::GetInstance().Log("Failed to load 64px font");
	}

	m_mediumFont = io.Fonts->AddFontFromFileTTF("joystix_monospace.otf", 36.0f);
	if (!m_mediumFont)
	{
		LogManager::GetInstance().Log("Failed to load 36px font");
	}

	m_smallFont = io.Fonts->AddFontFromFileTTF("joystix_monospace.otf", 14.0f);
	if (!m_smallFont)
	{
		LogManager::GetInstance().Log("Failed to load 14px font");
	}

	io.FontDefault = m_smallFont;

	ImGui_ImplOpenGL3_DestroyDeviceObjects();
	ImGui_ImplOpenGL3_CreateDeviceObjects();

	m_pWhiteTexture = new Texture();
	m_pWhiteTexture->SetID(CreateWhiteTexture());

	return initialised;
}

bool Renderer::InitialiseOpenGL(int screenWidth, int screenHeight)
{
	m_iWidth = screenWidth;
	m_iHeight = screenHeight;

	m_pWindow = SDL_CreateWindow("Pyromaniac", SDL_WINDOWPOS_UNDEFINED,
						     	  SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	SDL_Surface* icon = IMG_Load("../assets/favicon.ico");
	if (icon != nullptr) {
		SDL_SetWindowIcon(m_pWindow, icon);
		SDL_FreeSurface(icon);
	}
	else {
		SDL_Log("Failed to load icon: %s", SDL_GetError());
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	m_glContext = SDL_GL_CreateContext(m_pWindow);

	GLenum glewResult = glewInit();

	if (glewResult != GLEW_OK)
	{
		return false;
	}

	// Disable VSYNC
	SDL_GL_SetSwapInterval(0);

	bool shadersLoaded = SetupSpriteShader();

	return shadersLoaded;
}

void Renderer::Clear()
{
	int w, h;
	SDL_GetWindowSize(m_pWindow, &w, &h);
	m_iWidth = w;
	m_iHeight = h;

	glClearColor(m_fClearRed, m_fClearGreen, m_fClearBlue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void Renderer::Present()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(m_pWindow);
}

void Renderer::ToggleFullscreen()
{
	m_fullscreen = !m_fullscreen;

	if (m_fullscreen)
	{
		SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);

		SDL_DisplayMode displayMode;
		if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0)
		{
			m_iWidth = displayMode.w;
			m_iHeight = displayMode.h;
		}
	}
	else
	{
		SDL_SetWindowFullscreen(m_pWindow, 0);

		m_iWidth = 1280;
		m_iHeight = 720;

		SDL_SetWindowSize(m_pWindow, m_iWidth, m_iHeight);
		SDL_SetWindowPosition(m_pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}

void Renderer::SetClearColour(unsigned char r, unsigned char g, unsigned char b)
{
	m_fClearRed = r / 255.0f;
	m_fClearGreen = g / 255.0f;
	m_fClearBlue = b / 255.0f;
}

void Renderer::GetClearColour(unsigned char& r, unsigned char& g, unsigned char& b)
{
	r = static_cast<unsigned char>(m_fClearRed * 255.0f);
	g = static_cast<unsigned char>(m_fClearGreen * 255.0f);
	b = static_cast<unsigned char>(m_fClearBlue * 255.0f);
}

int Renderer::GetWidth() const
{
	return m_iWidth;
}

int Renderer::GetHeight() const
{
	return m_iHeight;
}

Sprite* Renderer::CreateSprite(const char* pcFilename)
{
	assert(m_pTextureManager);

	Texture* pTexture = m_pTextureManager->GetTexture(pcFilename);
	if (!pTexture)
	{
		LogManager::GetInstance().Log("Failed to load texture for sprite.");
		return nullptr;
	}

	Sprite* pSprite = new Sprite();
	if (!pSprite->Initialise(*pTexture))
	{
		LogManager::GetInstance().Log("Sprite Failed to Create!");
		delete pSprite;
		return nullptr;
	}

	return (pSprite);
}

void Renderer::LogSdlError()
{
	LogManager::GetInstance().Log(SDL_GetError());
}

bool Renderer::SetupSpriteShader()
{
	m_pSpriteShader = new Shader();

	bool loaded = m_pSpriteShader->Load("shaders\\sprite.vert", "shaders\\sprite.frag");

	m_pSpriteShader->SetActive();

	float vertices[] =
	{
	-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, // Top left
	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Top right
	0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom right
	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f // Bottom left
	};

	unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

	m_pSpriteVertexData = new VertexArray(vertices, 4, indices, 6);

	return loaded;
}

Renderer& Renderer::operator=(const Renderer& renderer)
{
	return *this;
}

void Renderer::DrawSprite(Sprite& sprite)
{
	m_pSpriteShader->SetActive();
	m_pSpriteVertexData->SetActive();

	float angleInDegrees = sprite.GetAngle();

	float sizeX = static_cast<float>(sprite.GetWidth());
	float sizeY = static_cast<float>(sprite.GetHeight());

	const float PI = 3.14159f;
	float angleInRadians = (angleInDegrees * PI) / 180.0f;

	Matrix4 world;
	SetIdentity(world);
	world.m[0][0] = cosf(angleInRadians) * (sizeX);
	world.m[0][1] = -sinf(angleInRadians) * (sizeX);
	world.m[1][0] = sinf(angleInRadians) * (sizeY);
	world.m[1][1] = cosf(angleInRadians) * (sizeY);
	world.m[3][0] = static_cast<float>(sprite.GetX());
	world.m[3][1] = static_cast<float>(sprite.GetY());

	m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);

	Matrix4 orthoViewProj;
	CreateOrthoProjection(orthoViewProj, static_cast<float>(m_iWidth), static_cast<float>(m_iHeight));

	m_pSpriteShader->SetVector4Uniform("colour", sprite.GetRedTint(),
		sprite.GetGreenTint(),
		sprite.GetBlueTint(),
		sprite.GetAlpha());

	m_pSpriteShader->SetMatrixUniform("uViewProj", orthoViewProj);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pSpriteShader->SetIntUniform("u_flipX", sprite.GetFlipX() ? 1 : 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

AnimatedSprite*
Renderer::CreateAnimatedSprite(const char* pcFilename)
{
	assert(m_pTextureManager);

	Texture* pTexture = m_pTextureManager->GetTexture(pcFilename);

	AnimatedSprite* pSprite = new AnimatedSprite();
	if (!pSprite->Initialise(*pTexture))
	{
		LogManager::GetInstance().Log("AnimatedSprite failed to create!");
		delete pSprite;
		return nullptr;
	}

	return pSprite;
}

void
Renderer::DrawAnimatedSprite(AnimatedSprite& sprite, int frame)
{
	m_pSpriteShader->SetActive();
	
	float angleInDegrees = sprite.GetAngle();
	float sizeX = static_cast<float>(sprite.GetWidth());
	float sizeY = static_cast<float>(sprite.GetHeight());
	
	const float PI = 3.14159f;
	float angleInRadians = (angleInDegrees * PI) / 180.0f;
	
	Matrix4 world;
	SetIdentity(world);
	world.m[0][0] = cosf(angleInRadians) * (sizeX);
	world.m[0][1] = -sinf(angleInRadians) * (sizeX);
	world.m[1][0] = sinf(angleInRadians) * (sizeY);
	world.m[1][1] = -cosf(angleInRadians) * (sizeY);
	world.m[3][0] = static_cast<float>(sprite.GetX());
	world.m[3][1] = static_cast<float>(sprite.GetY());
	
	m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);

	Matrix4 orthoViewProj;
	CreateOrthoProjection(orthoViewProj, static_cast<float>(m_iWidth), static_cast<float>(m_iHeight));

	m_pSpriteShader->SetVector4Uniform("colour", sprite.GetRedTint(),
	sprite.GetGreenTint(), sprite.GetBlueTint(), sprite.GetAlpha());
	m_pSpriteShader->SetMatrixUniform("uViewProj", orthoViewProj);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pSpriteShader->SetIntUniform("u_flipX", sprite.GetFlipX() ? 1 : 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((frame * 6) * sizeof(GLuint)));
}

void
Renderer::CreateStaticText(const char* pText, int pointsize)
{
	Texture* pTexture = new Texture();
	pTexture->LoadTextTexture(pText, "joystix_monospace.otf", pointsize);
	m_pTextureManager->AddTexture(pText, pTexture);
}

void Renderer::DrawRect(int x, int y, float width, float height,
	float r, float g, float b, float a)
{
	// Set up a basic world transform
	Matrix4 world;
	SetIdentity(world);
	world.m[0][0] = width;
	world.m[1][1] = height;
	world.m[3][0] = static_cast<float>(x);
	world.m[3][1] = static_cast<float>(y);

	m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);
	m_pWhiteTexture->SetActive();

	Matrix4 orthoViewProj;
	CreateOrthoProjection(orthoViewProj, static_cast<float>(m_iWidth), static_cast<float>(m_iHeight));
	m_pSpriteShader->SetMatrixUniform("uViewProj", orthoViewProj);

	m_pSpriteShader->SetVector4Uniform("colour", r, g, b, a);

	m_pSpriteVertexData->SetActive();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

GLuint Renderer::CreateWhiteTexture()
{
	GLuint texID;
	unsigned char whitePixel[4] = { 255, 255, 255, 255 }; // RGBA white

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return texID;
}