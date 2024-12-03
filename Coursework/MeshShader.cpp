#include "MeshShader.h"

MeshShader::MeshShader(ID3D11Device* device, HWND hwnd, VertexShaderType type) : BaseShader(device, hwnd)
{
	//each different type uses a different combination of shaders, as an improvement i could seperate out the depth buffers

	switch (type)
	{
	case waveType:
		type = waveType;
		initWaveShader(L"wave_vs.cso", L"light_ps.cso");
		break;
	case heightMapType:
		type = heightMapType;
		initHeightmapShader(L"heightMap_vs.cso", L"light_ps.cso");
		break;
	case basicType:
		type = basicType;
		initBasicShader(L"basic_vs.cso", L"light_ps.cso");
		break;
	case waveDepthPassType:
		type = waveDepthPassType;
		initWaveDepthShader(L"wave_depth_vs.cso", L"depth_ps.cso");
		break;
	case heightMapDepthPassType:
		type = heightMapDepthPassType;
		initHeightmapDepthShader(L"heightMap_Depth_vs.cso", L"depth_ps.cso");
		break;
	case basicDepthPassType:
		type = basicDepthPassType;
		initBasicDepthShader(L"basic_depth_vs.cso", L"depth_ps.cso");
		break;
	};

}

MeshShader::~MeshShader()
{
	if (heightMapBuffer) {
		heightMapBuffer->Release();
		heightMapBuffer = 0;
	}

	if (heightmapSamplerState) {
		heightmapSamplerState->Release();
		heightmapSamplerState = 0;
	}

	if (waveBuffer) {
		waveBuffer->Release();
		waveBuffer = 0;
	}

	if (cameraBuffer){
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	if (waveBuffer) {
		waveBuffer->Release();
		waveBuffer = 0;
	}

	if (lightBuffer) {
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	BaseShader::~BaseShader();

}

void MeshShader::initShader(const wchar_t* cs, const wchar_t* ps)
{
}

//the inits set up the buffer descriptions, setting the size and other settings since it will 
//be a stream sent to the gpu and unpacked again. one description per buffer

void MeshShader::initHeightmapShader(const wchar_t* cs, const wchar_t* ps)
{

	//remove texture if it sits in lighting


	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC heightMapBufferDesc;
	D3D11_SAMPLER_DESC heightmapSamplerDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	// Load (+ compile) shader files (might not need pixel shader)
	loadVertexShader(cs);
	//loadPixelShader(ps);

	//currently using baseshader matrixBuffer might need changed to own one
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	heightmapSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	heightmapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	heightmapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	heightmapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	heightmapSamplerDesc.MipLODBias = 0.0f;
	heightmapSamplerDesc.MaxAnisotropy = 1;
	heightmapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	heightmapSamplerDesc.MinLOD = 0;
	heightmapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&heightmapSamplerDesc, &heightmapSamplerState);

	heightMapBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightMapBufferDesc.ByteWidth = sizeof(HeightBufferType);
	heightMapBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	heightMapBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightMapBufferDesc.MiscFlags = 0;
	heightMapBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&heightMapBufferDesc, NULL, &heightMapBuffer);

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(HeightBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	initLightShader(ps);
}

void MeshShader::initWaveShader(const wchar_t* cs, const wchar_t* ps)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC waveBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	loadVertexShader(cs);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	waveBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waveBufferDesc.ByteWidth = sizeof(WaveBufferType);
	waveBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waveBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waveBufferDesc.MiscFlags = 0;
	waveBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&waveBufferDesc, NULL, &waveBuffer);

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(HeightBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	initLightShader(ps);
}

void MeshShader::initBasicShader(const wchar_t* cs, const wchar_t* ps)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	loadVertexShader(cs);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(HeightBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);

	initLightShader(ps);
}

void MeshShader::initHeightmapDepthShader(const wchar_t* cs, const wchar_t* ps)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC heightMapBufferDesc;
	D3D11_SAMPLER_DESC heightmapSamplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(cs);
	loadPixelShader(ps);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	heightmapSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	heightmapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	heightmapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	heightmapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	heightmapSamplerDesc.MipLODBias = 0.0f;
	heightmapSamplerDesc.MaxAnisotropy = 1;
	heightmapSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	heightmapSamplerDesc.MinLOD = 0;
	heightmapSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&heightmapSamplerDesc, &heightmapSamplerState);

	heightMapBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightMapBufferDesc.ByteWidth = sizeof(HeightBufferType);
	heightMapBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	heightMapBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightMapBufferDesc.MiscFlags = 0;
	heightMapBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&heightMapBufferDesc, NULL, &heightMapBuffer);
}

void MeshShader::initWaveDepthShader(const wchar_t* cs, const wchar_t* ps)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC waveBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(cs);
	loadPixelShader(ps);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	waveBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waveBufferDesc.ByteWidth = sizeof(WaveBufferType);
	waveBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waveBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waveBufferDesc.MiscFlags = 0;
	waveBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&waveBufferDesc, NULL, &waveBuffer);
}

void MeshShader::initBasicDepthShader(const wchar_t* cs, const wchar_t* ps)
{
	D3D11_BUFFER_DESC matrixBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(cs);
	loadPixelShader(ps);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
}

//the sets are used each time the scene is rendered, filling the buffers with the relevent info with the structs that are the same as the structs in the hlsl code
//setting the buffers, resources and samplers in specific slots
//the depth shaders so not have any inits for the pixel shader as theres no buffers for it, inly inputs from the vertex shaders
void MeshShader::setHeightmapShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, 
	const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, 
	ID3D11ShaderResourceView* heightmapTexture, ID3D11ShaderResourceView* depthMap, XMFLOAT3 cameraPosition, Light* light[3])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	XMMATRIX tLightViewMatrix = XMMatrixTranspose(light[0]->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light[0]->getOrthoMatrix());

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	HeightBufferType* heightPtr;
	deviceContext->Map(heightMapBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	heightPtr = (HeightBufferType*)mappedResource.pData;
	heightPtr->heightMultiplier = heightMulti;
	heightPtr->vectorLenth = vecLength;
	heightPtr->yEdit = yNormalEdit;
	heightPtr->padding = 0;
	deviceContext->Unmap(heightMapBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &heightMapBuffer);

	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = cameraPosition;
	cameraPtr->padding = 0;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &cameraBuffer);

	deviceContext->VSSetShaderResources(0, 1, &heightmapTexture);
	deviceContext->VSSetSamplers(0, 1, &heightmapSamplerState);

	//temp still might mess it up royaly but we will find out
	setLightShaderParameters(deviceContext, world, view, projection, texture, depthMap, light);
}

void MeshShader::setHeightmapDepthShaderParamiters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, 
	const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightmapTexture)
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

	HeightBufferType* heightPtr;
	deviceContext->Map(heightMapBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	heightPtr = (HeightBufferType*)mappedResource.pData;
	heightPtr->heightMultiplier = heightMulti;
	heightPtr->vectorLenth = vecLength;
	heightPtr->yEdit = yNormalEdit;
	heightPtr->padding = 0;
	deviceContext->Unmap(heightMapBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &heightMapBuffer);

	deviceContext->VSSetShaderResources(0, 1, &heightmapTexture);
	deviceContext->VSSetSamplers(0, 1, &heightmapSamplerState);
}

void MeshShader::setWaveShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, 
	const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float dt, XMFLOAT3 cameraPosition, Light* light[3])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	//shift up so it intersects with landscape
	XMMATRIX move = XMMatrixTranslation(0, 1, 0);
	//XMMATRIX resize = XMMatrixScaling(0.5f, 1.0f, 0.5f);
	//XMMATRIX tempWorld = XMMatrixMultiply(world, resize);
	XMMATRIX finalWorld = XMMatrixMultiply(world, move);

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(finalWorld);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(light[0]->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light[0]->getOrthoMatrix());

	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);


	timeInput += dt * speed;
	WaveBufferType* wavePtr;
	deviceContext->Map(waveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	wavePtr = (WaveBufferType*)mappedResource.pData;
	wavePtr->waveAmplitude = amp;
	wavePtr->waveFrequency = freq;
	wavePtr->time = timeInput;
	wavePtr->steepness = 1.0f;
	deviceContext->Unmap(waveBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &waveBuffer);

	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = cameraPosition;
	cameraPtr->padding = 0;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &cameraBuffer);

	setLightShaderParameters(deviceContext, world, view, projection, texture, depthMap, light);
}

void MeshShader::setWaveDepthShaderParamiters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, float dt)
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

	timeInput += dt * speed;
	WaveBufferType* wavePtr;
	deviceContext->Map(waveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	wavePtr = (WaveBufferType*)mappedResource.pData;
	wavePtr->waveAmplitude = amp;
	wavePtr->waveFrequency = freq;
	wavePtr->time = timeInput;
	wavePtr->steepness = 1.0f;
	deviceContext->Unmap(waveBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &waveBuffer);

}

void MeshShader::setBasicShaderParamiters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, XMFLOAT3 cameraPosition, Light* light[3])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	XMMATRIX tLightViewMatrix = XMMatrixTranspose(light[0]->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light[0]->getOrthoMatrix());

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = cameraPosition;
	cameraPtr->padding = 0;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);

	setLightShaderParameters(deviceContext, world, view, projection, texture, depthMap, light);
}

void MeshShader::setBasicDepthShaderParamiters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection)
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
}

void MeshShader::initLightShader(const wchar_t* ps)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	loadPixelShader(ps);

	// Create a texture sampler state description.
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

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

}

void MeshShader::setLightShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, Light* light[3])
{
	// Send light data to pixel shader

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;

	for (int i = 0; i < 3; i++) {
		lightPtr->light[i].diffuse = light[i]->getDiffuseColour();
		lightPtr->light[i].ambient = light[i]->getAmbientColour();
		lightPtr->light[i].specular = light[i]->getSpecularColour();

		lightPtr->light[i].direction = light[i]->getDirection();
		lightPtr->light[i].PenumbraAngle = 0;
		lightPtr->light[i].position = light[i]->getPosition();
		lightPtr->light[i].coneAngle = 0.0f;

		lightPtr->light[i].specularpower = light[i]->getSpecularPower();
		lightPtr->light[i].padding = XMFLOAT3(0, 0, 0);
	}

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}