// Vertical blur shader
#include "lightshader.h"


LightShader::LightShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"light_vs.cso", L"light_ps.cso");
}


LightShader::~LightShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout)
	{
		layout->Release();
		layout = 0;
	}
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void LightShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

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
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
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

	// Setup the description of the screen size.

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

}


void LightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap[3], Light* DirectionalLights[2], Light* PointLights[1], Light* SpotLights[1], Camera* camera)
{
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
	lightPtr->PointLights[0].attenuation = XMFLOAT3(99999.f, 99999.f, 99999.f);
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

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	deviceContext->PSSetShaderResources(1, 3, depthMap);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}




