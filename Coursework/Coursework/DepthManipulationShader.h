//Depth manipulation shader
//Handles heightmap depth calculations for proper shadows
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;



class DepthManipulationShader : public BaseShader
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
	DepthManipulationShader(ID3D11Device* device, HWND hwnd);
	~DepthManipulationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap[9], Light* DirectionalLights[2], Light* PointLights[1], Light* SpotLights[1], Camera* camera);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
};