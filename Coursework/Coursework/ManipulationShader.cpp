#include "ManipulationShader.h"

ManipulationShader::ManipulationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"manipulation_vs.cso", L"tesselation_hs.cso", L"tesselation_ds.cso", L"manipulation_ps.cso");
}


ManipulationShader::~ManipulationShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

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

	// Release the light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	if (cameraBuffer) 
	{
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void ManipulationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &cameraBuffer);
}

void ManipulationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void ManipulationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* depthMap[3], Light* DirectionalLights[2], Light* PointLights[1], Light* SpotLights[1], Camera* camera)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(DirectionalLights[0]->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(DirectionalLights[0]->getOrthoMatrix());


	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView[0] = tLightViewMatrix;
	dataPtr->lightProjection[0] = tLightProjectionMatrix;
	tLightViewMatrix = XMMatrixTranspose(DirectionalLights[1]->getViewMatrix());
	tLightProjectionMatrix = XMMatrixTranspose(DirectionalLights[1]->getOrthoMatrix());
	dataPtr->lightView[1] = tLightViewMatrix;
	dataPtr->lightProjection[1] = tLightProjectionMatrix;

	tLightViewMatrix = XMMatrixTranspose(PointLights[0]->getViewMatrix());
	tLightProjectionMatrix = XMMatrixTranspose(PointLights[0]->getProjectionMatrix());
	dataPtr->lightView[2] = tLightViewMatrix;
	dataPtr->lightProjection[2] = tLightProjectionMatrix;

	tLightViewMatrix = XMMatrixTranspose(SpotLights[0]->getViewMatrix());
	tLightProjectionMatrix = XMMatrixTranspose(SpotLights[0]->getProjectionMatrix());
	dataPtr->lightView[3] = tLightViewMatrix;
	dataPtr->lightProjection[3] = tLightProjectionMatrix;

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->HSSetConstantBuffers(1, 1, &matrixBuffer);

	//Additional
	// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;

	for (int i = 0; i < 2; ++i)
	{
		lightPtr->DirectionalLights[i].ambient = DirectionalLights[i]->getAmbientColour();
		lightPtr->DirectionalLights[i].diffuse = DirectionalLights[i]->getDiffuseColour();
		lightPtr->DirectionalLights[i].direction = DirectionalLights[i]->getDirection();
		lightPtr->DirectionalLights[i].padding = 0.f;
	}

	lightPtr->PointLights[0].ambient = PointLights[0]->getAmbientColour();
	lightPtr->PointLights[0].diffuse = PointLights[0]->getDiffuseColour();
	lightPtr->PointLights[0].position = PointLights[0]->getPosition();
	lightPtr->PointLights[0].attenuation = XMFLOAT3 (99999.f, 99999.f, 99999.f);
	lightPtr->PointLights[0].range = 14.f;
	lightPtr->PointLights[0].padding = 0.f;

	lightPtr->SpotLights[0].ambient = SpotLights[0]->getAmbientColour();
	lightPtr->SpotLights[0].diffuse = SpotLights[0]->getDiffuseColour();
	lightPtr->SpotLights[0].position = SpotLights[0]->getPosition();
	lightPtr->SpotLights[0].direction = SpotLights[0]->getDirection();
	lightPtr->SpotLights[0].attenuation = XMFLOAT3(0.00005f, 0.00005f, 0.00005f);
	lightPtr->SpotLights[0].spot = 50.f;
	lightPtr->SpotLights[0].range = 50.f;
	lightPtr->SpotLights[0].padding = 0.f;

	lightPtr->cameraPosition = camera->getPosition();
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	CameraBufferType* cameraPtr;
	deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPosition = camera->getPosition();
	cameraPtr->padding = 0.f;
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &cameraBuffer);
	deviceContext->HSSetConstantBuffers(0, 1, &cameraBuffer);


	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &texture2);

	if (depthMap != nullptr) {
		deviceContext->PSSetShaderResources(2, 3, depthMap);
		deviceContext->PSSetSamplers(0, 1, &sampleState);
		deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
	}


	// Set shader texture resource in the vertex shader.
	deviceContext->VSSetShaderResources(0, 1, &texture);
	deviceContext->VSSetSamplers(0, 1, &sampleState);

	deviceContext->HSSetShaderResources(0, 1, &texture);
	deviceContext->HSSetSamplers(0, 1, &sampleState);


}
