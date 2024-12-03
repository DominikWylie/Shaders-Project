#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

//i kept it mostly in one class so i can reuse the lighting shader and 
//there is less duplicated code and things that could go out of psync
//each enum corrosponds to a different shader config some use the same lighting shader and others use the same depth shader
enum VertexShaderType {
	waveType,
	heightMapType,
	basicType,
	waveDepthPassType,
	heightMapDepthPassType,
	basicDepthPassType
};

class MeshShader : public BaseShader
{
private:

	//the different structs for the different buffer types
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct HeightBufferType {
		float heightMultiplier;
		float vectorLenth;
		float yEdit;
		float padding;
	};

	struct WaveBufferType {
		float waveAmplitude;
		float waveFrequency;
		float time;
		float steepness;
	};

	struct CameraBufferType {
		XMFLOAT3 cameraPosition;
		float padding;
	};

	struct LightStruct {
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT4 specular;

		XMFLOAT3 direction;
		float PenumbraAngle;
		XMFLOAT3 position;
		float coneAngle;

		float specularpower;
		XMFLOAT3 padding;
	};

	struct LightBufferType
	{
		LightStruct light[3];
	};

public:
	MeshShader(ID3D11Device* device, HWND hwnd, VertexShaderType type);

	~MeshShader();

	//runs the normal coloured heightmap
	void setHeightmapShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world,
		const XMMATRIX& view, const XMMATRIX& projection,
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* vertexTexture, ID3D11ShaderResourceView* depthMap, XMFLOAT3 cameraPosition, Light* light[3]);

	//runs the depth values and returns them as a texture
	void setHeightmapDepthShaderParamiters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world,
		const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* vertexTexture);

	//needs to be public to manilulate with imgui
	float heightMulti = 100.0f, vecLength = 0.006f, normalYDebug = 0.0f, yNormalEdit = 0.2f;

	//runs the normal coloured heightmap
	void setWaveShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world,
		const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float dt, XMFLOAT3 cameraPosition, Light* light[3]);

	//runs the depth values and returns them as a texture
	void setWaveDepthShaderParamiters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world,
		const XMMATRIX& view, const XMMATRIX& projection, float dt);

	//needs to be public to manilulate with imgui
	float amp = 0.4f, freq = 0.4f, speed = 2.5f;

	//runs the normal coloured heightmap, the most basic of shaders
	void setBasicShaderParamiters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world,
		const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, XMFLOAT3 cameraPosition, Light* light[3]);

	//runs the depth values and returns them as a texture
	void setBasicDepthShaderParamiters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world,
		const XMMATRIX& view, const XMMATRIX& projection);

private:
	//i have my own inits for each shader configuration, exists for compatability
	void initShader(const wchar_t* cs, const wchar_t* ps);

	//initialisations of each shader - setting buffer descriptions for the corrosponding buffers in the shaders and loading shaders with the base shader
	void initLightShader(const wchar_t* ps);
	//is only called by the  other shaders
	void setLightShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world,
		const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, Light* light[3]);

	//initialisations of each shader - setting buffer descriptions for the corrosponding buffers in the shaders and loading shaders with the base shader
	void initHeightmapShader(const wchar_t* cs, const wchar_t* ps);
	//initialisations of each shader - setting buffer descriptions for the corrosponding buffers in the shaders and loading shaders with the base shader
	void initWaveShader(const wchar_t* cs, const wchar_t* ps);
	//initialisations of each shader - setting buffer descriptions for the corrosponding buffers in the shaders and loading shaders with the base shader
	void initBasicShader(const wchar_t* cs, const wchar_t* ps);

	//initialisations of each shader - setting buffer descriptions for the corrosponding buffers in the shaders and loading shaders with the base shader
	void initHeightmapDepthShader(const wchar_t* cs, const wchar_t* ps);
	//initialisations of each shader - setting buffer descriptions for the corrosponding buffers in the shaders and loading shaders with the base shader
	void initWaveDepthShader(const wchar_t* cs, const wchar_t* ps);
	//initialisations of each shader - setting buffer descriptions for the corrosponding buffers in the shaders and loading shaders with the base shader
	void initBasicDepthShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* heightMapBuffer;
	ID3D11SamplerState* heightmapSamplerState;

	ID3D11Buffer* cameraBuffer;

	ID3D11Buffer* waveBuffer;
	float timeInput = 0.0f;

	ID3D11SamplerState* sampleStateShadow;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;

	//enum type used in the constructor
	VertexShaderType type;
};

