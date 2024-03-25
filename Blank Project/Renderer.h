#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/Frustrum.h"

class Camera;
class HeightMap;
class SceneNode;
class Light;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene()				override;
	 void UpdateScene(float msec)	override;

	 void ToggleCamTrack() {
		cameraTrack = !cameraTrack;
		transition = false;
	 }

	 void ToggleView() { landscapeView = !landscapeView; }

	 void ToggleTransition() {
		transition = !transition;
		cameraTrack = false;
	 }
protected:
	void DrawSkybox();
	void DrawHeightMap();
	void DrawWater();
	void DrawObjects(SceneNode* node);
	void DrawLightObjects(SceneNode* node);
	void DrawSpaceObjects(SceneNode* node);
	void DrawOutHUD();
	void DrawInHUD();

	bool landscapeView;
	bool cameraTrack;
	bool transition;

	float lerpFloat(float initalValue, float endValue, float ratio);
	Vector4 lerpColour(Vector4 startColour, Vector4 endColour, float ratio);
	void DayCycle();

	void RotateParentNode(SceneNode* node, float dt, float degrees, Vector3 axis);

	void landCameraTrack(float rotate);
	void spaceCameraTrack(float rotate);
	void transitionOutTrack(float duration);
	void transitionInTrack(float duration);
	float camRotate;
	float duration;

	HeightMap* heightMap;
	Camera* camera;
	Mesh* skybox;
	Mesh* water;
	Mesh* HUD;

	SceneNode* sunAndMoon;
	SceneNode* objects;
	SceneNode* landViewAstroids;
	SceneNode* lightObjects;
	SceneNode* planets;
	SceneNode* spaceViewAstroids;
	SceneNode* spaceLightObjects;

	Shader* skyboxShader;
	Shader* reflectShader;
	Shader* landLightShader;
	Shader* noLightAffectShader;
	Shader* hudShader;

	float waterMovement;
	float waterRotation;
	
	Light* landLight;
	float dayRotation;
	float daySpeed;
	bool day;

	Light* spaceLight;

	GLuint terrainTexture;
	GLuint barkTexture;
	GLuint leafTexture;
	GLuint pathTexture;
	GLuint waterTexture;
	GLuint sunTexture;
	GLuint moonTexture;
	GLuint spaceSunTexture;
	GLuint planetTexture;
	GLuint cloudTexture;

	GLuint landCubeMap;
	GLuint spaceCubeMap;
};
