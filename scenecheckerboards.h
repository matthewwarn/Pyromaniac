// COMP710 GP Framework 2022

#ifndef SCENECHECKERBOARDS_H
#define SCENECHECKERBOARDS_H

// Local includes:
#include "scene.h"

// Forward declarations:
class Renderer;
class Sprite;

// Class declaration:
class SceneCheckerboards : public Scene
{
// Member methods:
public:
SceneCheckerboards();
virtual ~SceneCheckerboards();

virtual bool Initialise(Renderer& renderer);
virtual void Draw(Renderer& renderer);
virtual void DebugDraw();
virtual void Process(float deltaTime, InputSystem& inputSystem); 

protected:

private:
SceneCheckerboards(const SceneCheckerboards& sceneCheckerboards);
SceneCheckerboards& operator=(const SceneCheckerboards& sceneCheckerboards);

// Member data:
public:

protected:
Sprite* m_pCorners[4];
Sprite* m_pCentre;

float m_angle;
float m_rotationSpeed;

private:

};

#endif // SCENECHECKERBOARDS_H