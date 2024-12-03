#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class MotionBlurShader : public BaseShader
{
private:
	//struct for the buffer sent in to the shader
	struct motionBlurMatrixBufferType 
	{
		XMMATRIX inverseView;
		XMMATRIX previousView;
		float blurStrength;
		XMFLOAT3 padding;
	};

public:
	MotionBlurShader(ID3D11Device* device, HWND hwnd);
	~MotionBlurShader();

	//set is used evrey time its rendered to fill the buffers
	void setMotionBlurShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, 
		const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float strength);

	//setting the matrixes seperately as they need set outside of the post processing ortho mesh
	void setPreviousMatrix(XMMATRIX previousView);
	void setInverseView(XMMATRIX inverseView);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

	ID3D11SamplerState* sampleState;

	ID3D11Buffer* motionBlurBuffer;

	XMMATRIX previousViewMatrix = XMMatrixIdentity();
	XMMATRIX inverseViewMatrix = XMMatrixIdentity();

};

