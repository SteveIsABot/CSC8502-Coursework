#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/heightMap.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Light.h"

#include "../nclgl/Palmtree.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {

	heightMap = new HeightMap(TEXTUREDIR"noiseTexture.png");
	skybox = Mesh::GenerateQuad();
	water = Mesh::GenerateQuad();
	HUD = Mesh::GenerateQuad();

	terrainTexture = SOIL_load_OGL_texture(TEXTUREDIR"sandTexture.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	barkTexture = SOIL_load_OGL_texture(TEXTUREDIR"palmTree.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	leafTexture = SOIL_load_OGL_texture(TEXTUREDIR"leaves.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	pathTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	waterTexture = SOIL_load_OGL_texture(TEXTUREDIR"waterTexture.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	sunTexture = SOIL_load_OGL_texture(TEXTUREDIR"sun.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	moonTexture = SOIL_load_OGL_texture(TEXTUREDIR"moon.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	spaceSunTexture = SOIL_load_OGL_texture(TEXTUREDIR"spaceSun.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	planetTexture = SOIL_load_OGL_texture(TEXTUREDIR"planet.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	cloudTexture = SOIL_load_OGL_texture(TEXTUREDIR"clouds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	landCubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"sky-north.jpg", TEXTUREDIR"sky-south.jpg", TEXTUREDIR"sky-up.jpg",
										TEXTUREDIR"sky-down.jpg", TEXTUREDIR"sky-east.jpg", TEXTUREDIR"sky-west.jpg",
										SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	spaceCubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"space-east.png", TEXTUREDIR"space-west.png", TEXTUREDIR"space-up.png",
										 TEXTUREDIR"space-down.png", TEXTUREDIR"space-north.png", TEXTUREDIR"space-south.png",
										 SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if(!terrainTexture || !barkTexture || !leafTexture || !pathTexture || !waterTexture || !sunTexture || !moonTexture || !spaceSunTexture ||
	   !planetTexture || !cloudTexture || !landCubeMap || !spaceCubeMap) return;

	SetTextureRepeating(terrainTexture, true);
	SetTextureRepeating(barkTexture, true);
	SetTextureRepeating(leafTexture, true);
	SetTextureRepeating(pathTexture, true);
	SetTextureRepeating(waterTexture, true);
	SetTextureRepeating(sunTexture, true);
	SetTextureRepeating(moonTexture, true);
	SetTextureRepeating(spaceSunTexture, true);
	SetTextureRepeating(planetTexture, true);
	SetTextureRepeating(cloudTexture, true);

	Vector3 dimension = heightMap->GetHeightmapSize();
	camera = new Camera();
	camera->SetPosition(dimension * Vector3(0.5f, 2.0f, 0.2f));

	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	landLightShader = new Shader("perPixelVertex.glsl", "perPixelFragment.glsl");
	noLightAffectShader = new Shader("sceneVertex.glsl", "sceneFragment.glsl");
	hudShader = new Shader("hudVertex.glsl", "hudFragment.glsl");

	if(!skyboxShader->LoadSuccess() || !reflectShader->LoadSuccess() || !landLightShader->LoadSuccess() || !noLightAffectShader->LoadSuccess() || !hudShader->LoadSuccess()) return;

	landLight = new Light(dimension * Vector3(5.0f, 0.0f, 0.5f), Vector4(1, 1, 1, 1), dimension.x * 2);
	dayRotation = 0;
	daySpeed = 0.5;
	day = true;

	objects = new SceneNode();
	objects->SetTransform(Matrix4::Translation(dimension * Vector3(0.5f, 1.0f, 0.5f)));
	landViewAstroids = new SceneNode();
	landViewAstroids->SetTransform(Matrix4::Translation(dimension * Vector3(0.5f, 1.0f, 0.5f)));
	
	SceneNode* treeA = new Palmtree(Mesh::LoadFromMeshFile("Cone.msh"), Mesh::LoadFromMeshFile("Sphere.msh"), barkTexture, leafTexture);
	treeA->SetTransform(Matrix4::Translation(Vector3((dimension.x / 2.0f) - 500.0f, dimension.y, (dimension.z / 2.0f) - 500.0f)));
	SceneNode* treeB = new Palmtree(Mesh::LoadFromMeshFile("Cone.msh"), Mesh::LoadFromMeshFile("Sphere.msh"), barkTexture, leafTexture);
	treeB->SetTransform(Matrix4::Translation(Vector3((-dimension.x / 2.0f) + 1000.0f, dimension.y, (dimension.z / 2.0f) - 500.0f)));
	SceneNode* treeC = new Palmtree(Mesh::LoadFromMeshFile("Cone.msh"), Mesh::LoadFromMeshFile("Sphere.msh"), barkTexture, leafTexture);
	treeC->SetTransform(Matrix4::Translation(Vector3((-dimension.x / 2.0f) + 1000.0f, dimension.y, (-dimension.z / 2.0f) + 500.0f)));
	SceneNode* treeD = new Palmtree(Mesh::LoadFromMeshFile("Cone.msh"), Mesh::LoadFromMeshFile("Sphere.msh"), barkTexture, leafTexture);
	treeD->SetTransform(Matrix4::Translation(Vector3((dimension.x / 2.0f) - 500.0f, dimension.y, (-dimension.z / 2.0f) + 500.0f)));

	objects->AddChild(treeA);
	objects->AddChild(treeB);
	objects->AddChild(treeC);
	objects->AddChild(treeD);

	for(int i = 0; i < 8; i++) {
		SceneNode* path = new SceneNode();
		path->SetTransform(Matrix4::Translation(Vector3((-dimension.x / 10.0f * i) + dimension.x / 2.0f - dimension.x / 15.0f, -130.0f, 0)));
		path->SetModelScale(Vector3(dimension.x/15.0f, 120, dimension.z / 15.0f));
		path->SetMesh(Mesh::LoadFromMeshFile("OffsetCube.msh"));
		path->SetTexture(pathTexture);
		objects->AddChild(path);
	}

	for (int i = 0; i < 2; i++) {
		SceneNode* astroid = new SceneNode();
		astroid->SetTransform(Matrix4::Translation(Vector3(0, (dimension.y * 10.0f * cos(180.0f * i * (PI / 180))), (dimension.z * 10.0f * sin(180.0f * i * (PI / 180))))));
		astroid->SetModelScale(Vector3(75.0f, 75.0f, 75.0f));
		astroid->SetColour(Vector4(1, 1, 1, 1));
		astroid->SetMesh(Mesh::LoadFromMeshFile("Cylinder.msh"));
		astroid->SetTexture(pathTexture);
		landViewAstroids->AddChild(astroid);
	}

	lightObjects = new SceneNode();
	lightObjects->SetTransform(Matrix4::Translation(dimension * Vector3(0.5f, 1.0f, 0.2f)));

	sunAndMoon = new SceneNode();
	sunAndMoon->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	sunAndMoon->SetColour(Vector4(1,1,1,1));
	sunAndMoon->SetTransform(Matrix4::Translation(landLight->GetPosition()));
	sunAndMoon->SetMesh(Mesh::LoadFromMeshFile("Sphere.msh"));
	sunAndMoon->SetTexture(sunTexture);
	lightObjects->AddChild(sunAndMoon);

	spaceLight = new Light(Vector3(-6000.0f, 0.0f, -6000.0f), Vector4(1, 1, 1, 1), 1000000.0f);

	planets = new SceneNode();
	planets->SetTransform(Matrix4::Rotation(30.0f, Vector3(0,0,1)));
	spaceViewAstroids = new SceneNode();
	spaceViewAstroids->SetTransform(Matrix4::Rotation(30.0f, Vector3(0, 0, 1)));
	spaceLightObjects = new SceneNode();

	SceneNode* mainP = new SceneNode();
	mainP->SetModelScale(Vector3(200.0f, 200.0f, 200.0f));
	mainP->SetColour(Vector4(1,1,1,1));
	mainP->SetMesh(Mesh::LoadFromMeshFile("Sphere.msh"));
	mainP->SetTexture(planetTexture);
	planets->AddChild(mainP);

	SceneNode* moon = new SceneNode();
	moon->SetTransform(Matrix4::Translation(Vector3(-1000.0f, 0.0f, 0.0f)));
	moon->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	moon->SetColour(Vector4(1, 1, 1, 1));
	moon->SetMesh(Mesh::LoadFromMeshFile("Sphere.msh"));
	moon->SetTexture(moonTexture);
	planets->AddChild(moon);

	for(int i = 0; i < 20; i++) {
		SceneNode* astroid = new SceneNode();
		astroid->SetTransform(Matrix4::Translation(Vector3(0, (500.0f * cos(18.0f * i * (PI / 180))), (500.0f * sin(18.0f * i * (PI / 180))))));
		astroid->SetModelScale(Vector3(25.0f, 25.0f, 25.0f));
		astroid->SetColour(Vector4(1,1,1,1));
		astroid->SetMesh(Mesh::LoadFromMeshFile("Cylinder.msh"));
		astroid->SetTexture(pathTexture);
		spaceViewAstroids->AddChild(astroid);
	}

	SceneNode* spaceSun = new SceneNode();
	spaceSun->SetTransform(Matrix4::Translation(Vector3(-6000.0f, 0.0f, -6000.0f)));
	spaceSun->SetModelScale(Vector3(1500.0f, 1500.0f, 1500.0f));
	spaceSun->SetMesh(Mesh::LoadFromMeshFile("Sphere.msh"));
	spaceSun->SetTexture(spaceSunTexture);
	spaceLightObjects->AddChild(spaceSun);

	waterRotation = 0.0f;
	waterMovement = 0.0f;
	
	camRotate = 0.0f;
	duration = 0.0f;

	landscapeView = true;
	cameraTrack = true;
	transition = false;

	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	init = true;
}
Renderer::~Renderer(void) {
	delete heightMap, camera, skybox, water, HUD, landLight, spaceLight;
	delete skyboxShader, reflectShader, landLight, noLightAffectShader, hudShader;
	delete sunAndMoon, objects, landViewAstroids, lightObjects, planets, spaceViewAstroids, spaceLightObjects;
	
	glDeleteTextures(1, &terrainTexture);
	glDeleteTextures(1, &barkTexture);
	glDeleteTextures(1, &leafTexture);
	glDeleteTextures(1, &pathTexture);
	glDeleteTextures(1, &waterTexture);
	glDeleteTextures(1, &sunTexture);
	glDeleteTextures(1, &moonTexture);
	glDeleteTextures(1, &spaceSunTexture);
	glDeleteTextures(1, &planetTexture);
	glDeleteTextures(1, &cloudTexture);
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();

	waterRotation += dt * 2.0f;
	waterMovement += dt * 0.25f;
	camRotate += dt * 5.0f;

	if(transition) {
		duration += 0.5f;
		landscapeView ? transitionOutTrack(duration) : transitionInTrack(duration);
	} else {
		duration = 0.0f;
	}

	if(landscapeView) {
		spaceLight->SetRadius(0.0f);
		landLight->SetRadius(heightMap->GetHeightmapSize().x * 2.0f);

		DayCycle();
		RotateParentNode(landViewAstroids, dt, 10, Vector3(1, 0, 0));

		lightObjects->Update(dt);
		objects->Update(dt);
		landViewAstroids->Update(dt);

		if(cameraTrack) landCameraTrack(camRotate);
	} else {
		landLight->SetRadius(0.0f);
		spaceLight->SetRadius(1000000.0f);

		RotateParentNode(spaceViewAstroids, dt, 5, Vector3(1, 0, 0));
		RotateParentNode(planets, dt, 10, Vector3(0, 1, 0));

		spaceLightObjects->Update(dt);
		planets->Update(dt);
		spaceViewAstroids->Update(dt);

		if(cameraTrack) spaceCameraTrack(camRotate);
	}
}

void Renderer::RenderScene() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	DrawSkybox();

	if(landscapeView) {
		DrawHeightMap();
		DrawWater();
		DrawObjects(objects);
		DrawObjects(landViewAstroids);
		DrawLightObjects(lightObjects);
	} else {
		DrawSpaceObjects(planets);
		DrawSpaceObjects(spaceViewAstroids);
		DrawLightObjects(spaceLightObjects);
	}

	if(transition) { 
		landscapeView ? DrawOutHUD() : DrawInHUD();
	}
}

void Renderer::DrawSkybox() {
	
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(skyboxShader->GetProgram(), "cubeTex"), 0);
	glActiveTexture(GL_TEXTURE0);

	!landscapeView ? glBindTexture(GL_TEXTURE_CUBE_MAP, spaceCubeMap) : glBindTexture(GL_TEXTURE_CUBE_MAP, landCubeMap);

	skybox->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightMap() {

	BindShader(landLightShader);
	SetShaderLight(*landLight);

	glUniform1i(glGetUniformLocation(landLightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTexture);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawWater() {
	
	BindShader(reflectShader);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, landCubeMap);

	Vector3 hSize = heightMap->GetHeightmapSize();
	Vector3 placement = Vector3(hSize.x * 0.125f, hSize.y * 0.5f, hSize.z * 0.5f);
	Vector3 size = Vector3(hSize.x * 0.125f, hSize.y * 0.5f, hSize.z * 0.5f);

	modelMatrix = Matrix4::Translation(placement) * Matrix4::Scale(size) * Matrix4::Rotation(-90, Vector3(1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3(waterMovement, 0.0f, waterMovement)) * Matrix4::Scale(Vector3(10, 10, 10)) *
					Matrix4::Rotation(waterRotation, Vector3(0, 0, 1));

	UpdateShaderMatrices();
	water->Draw();
}

void Renderer::DrawObjects(SceneNode* node) {
	
	if(node->GetMesh()) {

		BindShader(landLightShader);
		UpdateShaderMatrices();

		glUniform3fv(glGetUniformLocation(landLightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
		
		Matrix4 model = node->GetWorldTransform() * Matrix4::Scale(node->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(landLightShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(landLightShader->GetProgram(), "colour"), 1, (float*)&node->GetColour());

		GLuint texture = node->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(landLightShader->GetProgram(), "diffuseTex"), 0);

		node->Draw(*this);
	}

	for (vector<SceneNode*>::const_iterator i = node->GetChildrenIteratorStart(); i != node->GetChildrenIteratorEnd(); i++) {
		DrawObjects(*i);
	}
}

void Renderer::DrawLightObjects(SceneNode* node) {

	if (node->GetMesh()) {

		BindShader(noLightAffectShader);
		UpdateShaderMatrices();

		Matrix4 model = node->GetWorldTransform() * Matrix4::Scale(node->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(noLightAffectShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(noLightAffectShader->GetProgram(), "nodeColour"), 1, (float*)&node->GetColour());

		GLuint texture = node->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(glGetUniformLocation(noLightAffectShader->GetProgram(), "useTexture"), texture);

		node->Draw(*this);
	}

	for (vector<SceneNode*>::const_iterator i = node->GetChildrenIteratorStart(); i != node->GetChildrenIteratorEnd(); i++) {
		DrawLightObjects(*i);
	}
}

void Renderer::DrawSpaceObjects(SceneNode* node) {

	if (node->GetMesh()) {

		BindShader(landLightShader);
		SetShaderLight(*spaceLight);
		UpdateShaderMatrices();

		glUniform3fv(glGetUniformLocation(landLightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

		Matrix4 model = node->GetWorldTransform() * Matrix4::Scale(node->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(landLightShader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(landLightShader->GetProgram(), "colour"), 1, (float*)&node->GetColour());

		GLuint texture = node->GetTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(landLightShader->GetProgram(), "diffuseTex"), 0);

		node->Draw(*this);
	}

	for (vector<SceneNode*>::const_iterator i = node->GetChildrenIteratorStart(); i != node->GetChildrenIteratorEnd(); i++) {
		DrawSpaceObjects(*i);
	}
}

void Renderer::DrawOutHUD() {
	
	glDepthMask(GL_FALSE);
	
	BindShader(hudShader);

	Vector3 placement = camera->GetPosition();
	placement.y -= 1.5f;
	Vector3 size = Vector3(1.2f, 0.8f, 1.0f);

	float transparency = (placement.y - 2000.0f) / 950.0f;
	if(transparency >= 1.0f) transparency = 1.0f;
	if(placement.y <= 2000.0f) transparency = 0.0f;
	Vector4 colour = Vector4(1, 1, 1, transparency);

	glUniform4fv(glGetUniformLocation(hudShader->GetProgram(), "objColour"), 1, (float*)&colour);

	glUniform1i(glGetUniformLocation(hudShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cloudTexture);
	
	modelMatrix = Matrix4::Translation(placement) * Matrix4::Scale(size) * Matrix4::Rotation(-90, Vector3(1, 0, 0));
	UpdateShaderMatrices();
	HUD->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::DrawInHUD() {
	
	glDepthMask(GL_FALSE);

	BindShader(hudShader);

	Vector3 placement = camera->GetPosition();
	placement.x += 1.5f;
	placement.z += 1.5f;
	Vector3 size = Vector3(2.0f, 1.0f, 1.0f);

	float transparency = (1000.0f + placement.x) / 650.0f;
	if (transparency >= 1.0f) transparency = 1.0f;
	if (placement.x <= -1000.0f) transparency = 0.0f;
	Vector4 colour = Vector4(0, 0, 0, transparency);

	glUniform4fv(glGetUniformLocation(hudShader->GetProgram(), "objColour"), 1, (float*)&colour);

	glUniform1i(glGetUniformLocation(hudShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	modelMatrix = Matrix4::Translation(placement) * Matrix4::Rotation(-135, Vector3(0, 1, 0)) * Matrix4::Scale(size);
	UpdateShaderMatrices();
	HUD->Draw();

	glDepthMask(GL_TRUE);
}

float Renderer::lerpFloat(float initalValue, float endValue, float ratio) {
	return initalValue + (endValue - initalValue) * ratio;
}

Vector4 Renderer::lerpColour(Vector4 startColour, Vector4 endColour, float ratio) {

	float x = startColour.x + (endColour.x - startColour.x) * ratio;
	float y = startColour.y + (endColour.y - startColour.y) * ratio;
	float z = startColour.z + (endColour.z - startColour.z) * ratio;
	float w = startColour.w + (endColour.w - startColour.w) * ratio;

	return Vector4(x, y, z, w);
}

void Renderer::DayCycle() {

	dayRotation += daySpeed;
	dayRotation = std::fmod(dayRotation, 180);

	Vector3 dimensions = heightMap->GetHeightmapSize();
	Vector4 dayColour = Vector4(1, 1, 1, 1);
	Vector4 sunsetColour = Vector4(255.0f/255.0f, 186.0f/255.0f, 166.0f/255.0f, 1);
	Vector4 nightColour = Vector4(202.0f/255.0f, 233.0f/255.0f, 252.0f/255.0f, 1);

	if(dayRotation >= 179.5f) day = !day;
	
	if(!day) {
		landLight->SetRadius(dimensions.x);
		landLight->SetColour(nightColour);
		sunAndMoon->SetTexture(moonTexture);
	} else {

		if (dayRotation < 90) {
			landLight->SetRadius(dimensions.x * lerpFloat(1.5f, 2.0f, dayRotation /90));
			landLight->SetColour(lerpColour(sunsetColour, dayColour, dayRotation /90));
		} else if (dayRotation > 90) {
			landLight->SetRadius(dimensions.x * lerpFloat(2.0f, 1.5f, (dayRotation - 90)/120));
			landLight->SetColour(lerpColour(dayColour, sunsetColour, (dayRotation - 90)/120));
		}
		sunAndMoon->SetTexture(sunTexture);
	}

	float x = dimensions.x * cos(dayRotation * (PI/ 180));
	float y = dimensions.y * 10.0f * sin(dayRotation * (PI/ 180));
	float z = dimensions.z * 0.5f;

	sunAndMoon->SetColour(landLight->GetColour());
	sunAndMoon->SetTransform(Matrix4::Translation(landLight->GetPosition()));
	landLight->SetPosition(Vector3(x, y, z));
}

void Renderer::RotateParentNode(SceneNode* node, float dt, float degrees, Vector3 axis) {
	Matrix4 t = node->GetTransform();
	node->SetTransform(t * Matrix4::Rotation(degrees * dt, axis));
}

void Renderer::landCameraTrack(float rotate) {
	
	float radius = heightMap->GetHeightmapSize().x;
	float height = heightMap->GetHeightmapSize().y * 2.0f;
	Vector3 cameraStartPos = heightMap->GetHeightmapSize() * Vector3(0.5f, 2.0f, 0.5f);
	Vector3 cameraPos = Vector3(radius * cos(rotate * (PI/180)), height, radius * sin(rotate * (PI/180)));
	
	camera->SetPosition(cameraStartPos + cameraPos);
	viewMatrix = camera->LookAt(cameraStartPos, Vector3(0, 1, 0));
}

void Renderer::spaceCameraTrack(float rotate) {

	float radius = 1000.0f;
	Vector3 cameraStartPos = Vector3(0, 0, 0);
	Vector3 cameraPos = Vector3(radius * cos(rotate * (PI / 180)), 0.0f, radius * sin(rotate * (PI / 180)));

	camera->SetPosition(cameraPos);
	viewMatrix = camera->LookAt(cameraStartPos, Vector3(0, 1, 0));
}

void Renderer::transitionOutTrack(float duration) {

	Vector3 cameraPos = Vector3(heightMap->GetHeightmapSize() * Vector3(0.5f, 5.0f, 0.5f));
	cameraPos.y += duration * 10.0f;

	camera->SetPosition(cameraPos);
	viewMatrix = camera->LookAt(heightMap->GetHeightmapSize() * Vector3(0.5f, 2.0f, 0.5f), Vector3(0, 0, 1));

	if(cameraPos.y >= 3000.0f) {
		transition = !transition;
		ToggleView();
		camera->SetPosition(Vector3(-750, 0, -750));
	}
}

void Renderer::transitionInTrack(float duration) {

	Vector3 cameraPos = Vector3(-1200.0f, 0.0f, -1200.0f);
	cameraPos.x += duration * 10.0f;
	cameraPos.z += duration * 10.0f;

	camera->SetPosition(cameraPos);
	viewMatrix = camera->LookAt(Vector3(0, 0, 0), Vector3(0, 1, 0));

	if (cameraPos.x >= -300.0f && cameraPos.z >= -300.0f) {
		transition = !transition;
		ToggleView();
		camera->SetPosition(heightMap->GetHeightmapSize() * Vector3(0.5f, 3.0f, 0.5f));
	}
}