//Depth wave shader
//Handles wave depth calculations for proper shadows
#pragma once

#include "DXF.h"
#include "WaveShader.h"

using namespace std;
using namespace DirectX;



class DepthWaveShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[4];
		XMMATRIX lightProjection[4];
	};

public:

	struct TimeBufferType
	{
		Wave waves[6];
		float time;
		XMFLOAT3 padding;
	};

public:
	DepthWaveShader(ID3D11Device* device, HWND hwnd);
	~DepthWaveShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap[9], Light* DirectionalLights[2], Light* PointLights[1], Light* SpotLights[1], Timer* time, Wave* wave[6], Camera* camera);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
};