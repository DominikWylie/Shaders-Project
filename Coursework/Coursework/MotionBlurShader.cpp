#include "MotionBlurShader.h"

MotionBlurShader::MotionBlurShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"motionBlur_vs.cso", L"motionBlur_ps.cso");
}

MotionBlurShader::~MotionBlurShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	 
	//Release base shader components
	BaseShader::~BaseShader();
}

void MotionBlurShader::setMotionBlurShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, 
	const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float strength)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	motionBlurMatrixBufferType* blurPtr;

	deviceContext->Map(motionBlurBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	blurPtr = (motionBlurMatrixBufferType*)mappedResource.pData;
	//try both
	blurPtr->inverseView = XMMatrixTranspose(inverseViewMatrix);
	//blurPtr->inverseView = XMMatrixTranspose(inverseViewMatrix);
	//blurPtr->inverseView = XMMatrixInverse(nullptr, tview);
	blurPtr->previousView = XMMatrixTranspose(previousViewMatrix);
	//blurPtr->previousView = previousView;
	blurPtr->blurStrength = strength;
	blurPtr->padding = XMFLOAT3(0, 0, 0);
	deviceContext->Unmap(motionBlurBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &motionBlurBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	//previousView = view;

}

void MotionBlurShader::setPreviousMatrix(XMMATRIX pV)
{
	previousViewMatrix = pV;
}

void MotionBlurShader::setInverseView(XMMATRIX iV) {
	inverseViewMatrix = XMMatrixInverse(nullptr, iV);
}

void MotionBlurShader::initShader(const wchar_t* cs, const wchar_t* ps)
{
	//D3D11_BUFFER_DESC matrixBufferDesc;

	//matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	//matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//matrixBufferDesc.MiscFlags = 0;
	//matrixBufferDesc.StructureByteStride = 0;
	//renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	loadVertexShader(cs);
	loadPixelShader(ps);

	D3D11_BUFFER_DESC matrixBufferDesc;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_BUFFER_DESC motionBlurMatrixBufferDesc;

	motionBlurMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	motionBlurMatrixBufferDesc.ByteWidth = sizeof(motionBlurMatrixBufferType);
	motionBlurMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	motionBlurMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	motionBlurMatrixBufferDesc.MiscFlags = 0;
	motionBlurMatrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&motionBlurMatrixBufferDesc, NULL, &motionBlurBuffer);

	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}

