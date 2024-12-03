// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.

	//textureMgr->loadTexture(L"hills", L"res/Heightmap_06_Canyon.png");
	//textureMgr->loadTexture(L"height", L"res/Heightmap_06_Canyon.png");

	//textureMgr->loadTexture(L"hills", L"res/islandHeightmap.png");
	//textureMgr->loadTexture(L"height", L"res/islandHeightmap.png");
	//textureMgr->loadTexture(L"height", L"res/islandHeightmap.png");

	textureMgr->loadTexture(L"hills", L"res/HeightTexture.png");
	textureMgr->loadTexture(L"height", L"res/height.png");

	//textureMgr->loadTexture(L"hills", L"res/HillsCombined2.png");
	//textureMgr->loadTexture(L"height", L"res/HillsHeightSmoothed.png");
	textureMgr->loadTexture(L"water", L"res/water.png");
	
	//initialise different meshes
	landscape = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 1000);
	waterMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 1000);
	testSphere = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	pot = new AModel(renderer->getDevice(), "res/teapot.obj");
	for (int i = 0; i < lightAmount; i++) {
		lightCube[i] = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	}

	//initialise shaders
	waveMeshShader = new MeshShader(renderer->getDevice(), hwnd, waveType);
	heightmapMeshShader = new MeshShader(renderer->getDevice(), hwnd, heightMapType);
	//a basic shader for testing lighting and rendering basic models
	basicShader = new MeshShader(renderer->getDevice(), hwnd, basicType);

	//equivelent shaders to render depth for shadow maps and post processing, motion blur
	waveMeshDepthShader = new MeshShader(renderer->getDevice(), hwnd, waveDepthPassType);
	basicDepthShader = new MeshShader(renderer->getDevice(), hwnd, basicDepthPassType);
	heightMapMeshDepthShader = new MeshShader(renderer->getDevice(), hwnd, heightMapDepthPassType);

	//motion blur shader and corrosponding render textures
	motionBlurShader = new MotionBlurShader(renderer->getDevice(), hwnd);

	//texture for processing the motion blur on
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	//texture for capturing the depth before sending to ortho mesh
	cameraDepthTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	//ortho mesh for rendering final texture to screen
	motionBlurOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);

	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	//shadow map for shadows
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// Configure lighting, i have 3 lights, i only managed to get the 
	//shadow map to work with 1 light before running out of time
	//all lights work with diffuse, ambient and specular
	light[0] = new Light();
	light[0]->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	light[0]->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	light[0]->setDirection(1.0f, -1.0f, 0.0f);
	light[0]->setPosition(0.5f, 2.0f, 0.5f);
	light[0]->setSpecularColour(1, 0, 0, 1);
	light[0]->setSpecularPower(100);
	light[0]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	light[1] = new Light();
	light[1]->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	light[1]->setDiffuseColour(0.0f, 1.0f, 0.0f, 1.0f);
	light[1]->setDirection(0.8f, -0.8f, 0.7f);
	light[1]->setPosition(20.0f, 15.0f, 20.0f);
	light[1]->setSpecularColour(0, 1, 0, 1);
	light[1]->setSpecularPower(100);

	light[2] = new Light();
	light[2]->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	light[2]->setDiffuseColour(0.0f, 0.0f, 1.0f, 1.0f);
	light[2]->setDirection(0.8f, -0.8f, 0.7f);
	light[2]->setPosition(30.0f, 12.0f, 17.0f);
	light[2]->setSpecularColour(0, 0, 1, 1);
	light[2]->setSpecularPower(100);

	//init camera position
	camera->setPosition(30.0f, 10.0, -20.0f);

	//clock for waves
	prevFrameTime = clock.now();
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	
	if (landscape) {
		delete landscape;
		landscape = 0;
	}

	if (waterMesh) {
		delete waterMesh;
		waterMesh = 0;
	}

}

void App1::depthPass() {
	//delta time clock for waves
	currentFrameTime = clock.now();
	deltaTime = currentFrameTime - prevFrameTime;
	deltaTimeSeconds = deltaTime.count();

	//first pass is for shadow map, binding shadow map to depth buffer
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	//get matrixes of light
	light[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = light[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light[0]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	XMMATRIX scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);

	//manipulating the matrixes of the objects in the scene
	XMMATRIX sphereMatrix = XMMatrixTranslation(40, 9, 36);
	XMMATRIX potMatrixTranslate = XMMatrixTranslation(35, 9, 32);
	XMMATRIX potMatrixScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	XMMATRIX potMatrixFinal = XMMatrixMultiply(potMatrixScale, potMatrixTranslate);

	XMMATRIX lightCubeMatrix[lightAmount];
	for (int i = 0; i < lightAmount; i++) {
		lightCubeMatrix[i] = XMMatrixTranslation(light[i]->getPosition().x, light[i]->getPosition().y, light[i]->getPosition().z);
	}

	//render depth data to shadow map of each component
	landscape->sendData(renderer->getDeviceContext());
	heightMapMeshDepthShader->setHeightmapDepthShaderParamiters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"height"));
	heightMapMeshDepthShader->render(renderer->getDeviceContext(), landscape->getIndexCount());

	waterMesh->sendData(renderer->getDeviceContext());
	waveMeshDepthShader->setWaveDepthShaderParamiters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, deltaTimeSeconds);
	waveMeshDepthShader->render(renderer->getDeviceContext(), waterMesh->getIndexCount());

	pot->sendData(renderer->getDeviceContext());
	basicDepthShader->setBasicDepthShaderParamiters(renderer->getDeviceContext(), potMatrixFinal, lightViewMatrix, lightProjectionMatrix);
	basicDepthShader->render(renderer->getDeviceContext(), pot->getIndexCount());

	testSphere->sendData(renderer->getDeviceContext());
	basicDepthShader->setBasicDepthShaderParamiters(renderer->getDeviceContext(), sphereMatrix, lightViewMatrix, lightProjectionMatrix);
	basicDepthShader->render(renderer->getDeviceContext(), testSphere->getIndexCount());

	for (int i = 0; i < lightAmount; i++) {
		lightCube[i]->sendData(renderer->getDeviceContext());
		//basicShader->setBasicDepthShaderParamiters(renderer->getDeviceContext(), lightCubeMatrix[i], lightViewMatrix, lightProjectionMatrix);
		//basicShader->render(renderer->getDeviceContext(), lightCube[i]->getIndexCount());

		basicDepthShader->setBasicDepthShaderParamiters(renderer->getDeviceContext(), lightCubeMatrix[i], lightViewMatrix, lightProjectionMatrix);
		basicDepthShader->render(renderer->getDeviceContext(), lightCube[i]->getIndexCount());
	}

	//reset render to the default, screen
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}


void App1::blurDepthPass() {

	//get the depth values of the scene from the camera and render to a texture
	cameraDepthTexture->setRenderTarget(renderer->getDeviceContext());
	cameraDepthTexture->clearRenderTarget(renderer->getDeviceContext(), 0, 0, 0, 1);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	XMMATRIX scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX sphereMatrix = XMMatrixTranslation(40, 9, 36);
	XMMATRIX potMatrixTranslate = XMMatrixTranslation(35, 9, 32);
	XMMATRIX potMatrixScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	XMMATRIX potMatrixFinal = XMMatrixMultiply(potMatrixScale, potMatrixTranslate);

	XMMATRIX lightCubeMatrix[lightAmount];
	for (int i = 0; i < lightAmount; i++) {
		lightCubeMatrix[i] = XMMatrixTranslation(light[i]->getPosition().x, light[i]->getPosition().y, light[i]->getPosition().z);
	}

	//render each component depth to texture
	landscape->sendData(renderer->getDeviceContext());
	heightMapMeshDepthShader->setHeightmapDepthShaderParamiters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"height"));
	heightMapMeshDepthShader->render(renderer->getDeviceContext(), landscape->getIndexCount());

	waterMesh->sendData(renderer->getDeviceContext());
	waveMeshDepthShader->setWaveDepthShaderParamiters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, deltaTimeSeconds);
	waveMeshDepthShader->render(renderer->getDeviceContext(), waterMesh->getIndexCount());

	pot->sendData(renderer->getDeviceContext());
	basicDepthShader->setBasicDepthShaderParamiters(renderer->getDeviceContext(), potMatrixFinal, viewMatrix, projectionMatrix);
	basicDepthShader->render(renderer->getDeviceContext(), pot->getIndexCount());

	testSphere->sendData(renderer->getDeviceContext());
	basicDepthShader->setBasicDepthShaderParamiters(renderer->getDeviceContext(), sphereMatrix, viewMatrix, projectionMatrix);
	basicDepthShader->render(renderer->getDeviceContext(), testSphere->getIndexCount());

	for (int i = 0; i < lightAmount; i++) {
		lightCube[i]->sendData(renderer->getDeviceContext());

		basicDepthShader->setBasicDepthShaderParamiters(renderer->getDeviceContext(), lightCubeMatrix[i], viewMatrix, projectionMatrix);
		basicDepthShader->render(renderer->getDeviceContext(), lightCube[i]->getIndexCount());
	}

	//renderer->setBackBufferRenderTarget();
	//renderer->resetViewport();

}

void App1::blurTexturePass() {

	//render the normal scene to sceneTexture to get ready for the blur shader
	sceneTexture->setRenderTarget(renderer->getDeviceContext());
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	debug();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX scaleMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX sphereMatrix = XMMatrixTranslation(40, 9, 36);
	XMMATRIX potMatrixTranslate = XMMatrixTranslation(35, 9, 32);
	XMMATRIX potMatrixScale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
	XMMATRIX potMatrixFinal = XMMatrixMultiply(potMatrixScale, potMatrixTranslate);

	XMMATRIX lightCubeMatrix[lightAmount];
	for (int i = 0; i < lightAmount; i++) {
		lightCubeMatrix[i] = XMMatrixTranslation(light[i]->getPosition().x, light[i]->getPosition().y, light[i]->getPosition().z);
	}

	landscape->sendData(renderer->getDeviceContext());
	heightmapMeshShader->setHeightmapShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"hills"), textureMgr->getTexture(L"height"), shadowMap->getDepthMapSRV(), camera->getPosition(), light);
	heightmapMeshShader->render(renderer->getDeviceContext(), landscape->getIndexCount());

	waterMesh->sendData(renderer->getDeviceContext());
	waveMeshShader->setWaveShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), shadowMap->getDepthMapSRV(), deltaTimeSeconds, camera->getPosition(), light);
	waveMeshShader->render(renderer->getDeviceContext(), waterMesh->getIndexCount());

	testSphere->sendData(renderer->getDeviceContext());
	basicShader->setBasicShaderParamiters(renderer->getDeviceContext(), sphereMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"hills"), shadowMap->getDepthMapSRV(), camera->getPosition(), light);
	basicShader->render(renderer->getDeviceContext(), testSphere->getIndexCount());

	pot->sendData(renderer->getDeviceContext());
	basicShader->setBasicShaderParamiters(renderer->getDeviceContext(), potMatrixFinal, viewMatrix, projectionMatrix, textureMgr->getTexture(L"hills"), shadowMap->getDepthMapSRV(), camera->getPosition(), light);
	basicShader->render(renderer->getDeviceContext(), pot->getIndexCount());

	for (int i = 0; i < lightAmount; i++) {
		lightCube[i]->sendData(renderer->getDeviceContext());
		basicShader->setBasicShaderParamiters(renderer->getDeviceContext(), lightCubeMatrix[i], viewMatrix, projectionMatrix, textureMgr->getTexture(L"hills"), shadowMap->getDepthMapSRV(), camera->getPosition(), light);
		basicShader->render(renderer->getDeviceContext(), lightCube[i]->getIndexCount());
	}

	prevFrameTime = currentFrameTime;

	//reset the buffer to the screen
	renderer->setBackBufferRenderTarget();
}


void App1::finalPass() {

	//grab the inverse view as a seperate matrix because the matrices going in to the shader are flat orthographic
	motionBlurShader->setInverseView(camera->getViewMatrix());

	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrixFinal = renderer->getWorldMatrix();
	XMMATRIX orthoMatrixFinal = camera->getOrthoViewMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrixFinal = sceneTexture->getOrthoMatrix();	// Default camera position for orthographic rendering

	//render sceneTexture to the ortho mesh with the depth texture
	motionBlurOrthoMesh->sendData(renderer->getDeviceContext());
	motionBlurShader->setMotionBlurShaderParameters(renderer->getDeviceContext(), worldMatrixFinal, orthoMatrixFinal, orthoViewMatrixFinal, sceneTexture->getShaderResourceView(), cameraDepthTexture->getShaderResourceView(), blurStrength);
	motionBlurShader->render(renderer->getDeviceContext(), motionBlurOrthoMesh->getIndexCount());
	// Render GUI
	gui();

	renderer->endScene();

	//grab the previous matrix after rendering from the camera
	motionBlurShader->setPreviousMatrix(camera->getViewMatrix());

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	camera->update();

	//render depth for the shadow map
	depthPass();

	//render depth for the motion blur
	blurDepthPass();

	//render the scene for the motion blur (main pass)
	blurTexturePass();

	//render the moton blur and render to back buffer or screen
	finalPass();

	return true;
}

//lots of imgui options
void App1::debug() {
	light[0]->setPosition(LightPositionDebug1[0], LightPositionDebug1[1], LightPositionDebug1[2]);
	light[1]->setPosition(LightPositionDebug2[0], LightPositionDebug2[1], LightPositionDebug2[2]);
	light[2]->setPosition(LightPositionDebug3[0], LightPositionDebug3[1], LightPositionDebug3[2]);
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::SliderFloat("height map height", &heightmapMeshShader->heightMulti, 0, 1000);
	ImGui::SliderFloat("vectorlenth", &heightmapMeshShader->vecLength, 0.0001f, 0.01f);
	ImGui::SliderFloat("wave amplitude", &waveMeshShader->amp, 0, 3);
	ImGui::SliderFloat("wave frequency", &waveMeshShader->freq, 0, 3);
	ImGui::SliderFloat("wave speed", &waveMeshShader->speed, 0, 10);
	ImGui::SliderFloat3("light1", LightPositionDebug1, -20.0, 60.0);
	ImGui::SliderFloat3("light2", LightPositionDebug2, -20.0, 60.0);
	ImGui::SliderFloat3("light3", LightPositionDebug3, -20.0, 60.0);
	ImGui::SliderFloat("blur strength", &blurStrength, 0, 20);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

