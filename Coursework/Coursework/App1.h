// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include <chrono>;

#include "MeshShader.h"
#include "MotionBlurShader.h"

//#include <DirectXMath.h>


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass();
	void blurDepthPass();
	void blurTexturePass();
	void finalPass();
	void gui();

private:
	float LightPositionDebug1[3] = { 0.0f, 35.0f, 47.0f };
	float LightPositionDebug2[3] = { 20.0f, 15.0f, 20.0f };
	float LightPositionDebug3[3] = { 30.0f, 12.0f, 17.0f };

	float blurStrength = 1;

	
	float normalDebug = 0;

	void debug();
	static const int lightAmount = 3;
	Light* light[lightAmount];

	MeshShader* waveMeshShader;
	MeshShader* heightmapMeshShader;
	MeshShader* basicShader;

	MeshShader* waveMeshDepthShader;
	MeshShader* basicDepthShader;
	MeshShader* heightMapMeshDepthShader;

	PlaneMesh* landscape;
	PlaneMesh* waterMesh;

	SphereMesh* testSphere;
	AModel* pot;
	CubeMesh* lightCube[lightAmount];

	ShadowMap* shadowMap;

	MotionBlurShader* motionBlurShader;
	RenderTexture* sceneTexture;
	RenderTexture* cameraDepthTexture;
	OrthoMesh* motionBlurOrthoMesh;
	

	std::chrono::high_resolution_clock clock;
	std::chrono::time_point<std::chrono::high_resolution_clock> prevFrameTime, currentFrameTime;
	std::chrono::duration<float> deltaTime;
	float deltaTimeSeconds;
};

#endif