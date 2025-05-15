// COMP710 GP Framework 2021

// This include:
#include "animatedsprite.h"

// Local includes:
#include "renderer.h"
#include "vertexarray.h"
#include "texture.h"
#include "imgui/imgui.h"

// Library includes:
#include <cassert>

AnimatedSprite::AnimatedSprite()
	: m_pVertexData(0)
	, m_iFrameWidth(0)
	, m_iFrameHeight(0)
	, m_iCurrentFrame(0)
	, m_iTotalFrames(0)
	, m_fTimeElapsed(0.0f)
	, m_bAnimating(false)
	, m_bLooping(false)
	, m_frameDuration(1.0f)
	, totalTime(0.0f)
{

}

AnimatedSprite::~AnimatedSprite()
{
	delete m_pVertexData;
	m_pVertexData = 0;
}

bool
AnimatedSprite::Initialise(Texture& texture)
{
	return Sprite::Initialise(texture);
}

int
AnimatedSprite::GetWidth() const
{
	return static_cast<int>(ceilf(m_iFrameWidth * m_scale));
}

int
AnimatedSprite::GetHeight() const
{
	return static_cast<int>(ceilf(m_iFrameHeight * m_scale));
}

bool
AnimatedSprite::IsAnimating() const
{
	return m_bAnimating;
}

void AnimatedSprite::SetupFrames(int fixedFrameWidth, int fixedFrameHeight)
{
    m_iFrameWidth = fixedFrameWidth;
    m_iFrameHeight = fixedFrameHeight;

    const int texW = m_pTexture->GetWidth();
    const int texH = m_pTexture->GetHeight();
    const int cols = texW / fixedFrameWidth;
    const int rows = texH / fixedFrameHeight;
    m_iTotalFrames = cols * rows;

    const int vertsPer = 4;
    const int floatsPer = vertsPer * 5;      // x,y,z,u,v
    const int numVertices = vertsPer * m_iTotalFrames;
    float* vertices = new float[numVertices * 5];

    float uStep = 1.0f / float(cols);
    float vStep = 1.0f / float(rows);

    for (int row = 0; row < rows; ++row)
        for (int col = 0; col < cols; ++col)
        {
            int frameIdx = row * cols + col;
            int baseVert = frameIdx * floatsPer;

            // compute U extents
            float u0 = col * uStep;
            float u1 = (col + 1) * uStep;

            // compute V extents *flipped* so that row=0 is top of texture
            float v1 = 1.0f - float(row) * vStep;   // top edge
            float v0 = v1 - vStep;         // bottom edge

            // quad:  x,    y,   z,   u,   v
            float quad[] = {
                -0.5f,  0.5f, 0.0f, u0, v1,
                 0.5f,  0.5f, 0.0f, u1, v1,
                 0.5f, -0.5f, 0.0f, u1, v0,
                -0.5f, -0.5f, 0.0f, u0, v0
            };

            memcpy(&vertices[baseVert], quad, sizeof(quad));
        }

    // build indices
    const int totalIndices = 6 * m_iTotalFrames;
    unsigned int* indices = new unsigned int[totalIndices];
    for (int i = 0, v = 0; i < totalIndices; i += 6, v += 4)
    {
        indices[i + 0] = v + 0;
        indices[i + 1] = v + 1;
        indices[i + 2] = v + 2;
        indices[i + 3] = v + 2;
        indices[i + 4] = v + 3;
        indices[i + 5] = v + 0;
    }

    delete m_pVertexData;
    m_pVertexData = new VertexArray(vertices, numVertices, indices, totalIndices);

    delete[] vertices;
    delete[] indices;
}

void
AnimatedSprite::Process(float deltaTime)
{
	totalTime += deltaTime;
	
	if (m_bAnimating)
	{
		m_fTimeElapsed += deltaTime;
	
		if (m_fTimeElapsed > m_frameDuration)
		{
			++m_iCurrentFrame;
		
			if (m_iCurrentFrame >= m_iTotalFrames)
			{
				if (m_bLooping)
				{
					Restart();
				}
				else
				{
					m_iCurrentFrame = m_iTotalFrames - 1;
					m_bAnimating = false;
				}
			}
			m_fTimeElapsed = 0.0f;
		}
	}
}

void
AnimatedSprite::Draw(Renderer& renderer)
{
	assert(m_pVertexData);
	m_pTexture->SetActive();
	m_pVertexData->SetActive();
	renderer.DrawAnimatedSprite(*this, m_iCurrentFrame);
}

void AnimatedSprite::Animate
()
{
	m_bAnimating = true;
}

void
AnimatedSprite::SetFrameDuration(float seconds)
{
	m_frameDuration = seconds;
}

void
AnimatedSprite::SetLooping(bool loop)
{
	m_bLooping = loop;
}

void AnimatedSprite::Restart
()
{
	m_iCurrentFrame = 0;
	m_fTimeElapsed = 0.0f;
}

void AnimatedSprite::DebugDraw
()
{
	ImGui::SliderInt("Frame ", &m_iCurrentFrame, 0, m_iTotalFrames - 1);
}