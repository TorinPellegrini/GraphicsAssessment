//Wave shader
//Used to calculate gerstner wave vertex manipulation
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

struct Wave
{
	XMFLOAT2 direction;
	float steepness;
	float wavelength;
};

class WaveShader : public BaseShader
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

	struct LightBufferType
	{
		struct DirectionalLight
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT4 specular;

			XMFLOAT3 direction;
			float padding;
		}
		DirectionalLights[2];
		struct PointLight
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT4 specular;

			XMFLOAT3 position;
			float range;

			XMFLOAT3 attenuation;
			float padding;
		}
		PointLights[1];
		struct Spotlight
		{
			XMFLOAT4 ambient;
			XMFLOAT4 diffuse;
			XMFLOAT4 specular;

			XMFLOAT3 position;
			float range;

			XMFLOAT3 direction;
			float spot;

			XMFLOAT3 attenuation;
			float padding;
		}
		SpotLights[1];

		XMFLOAT3 cameraPosition;
		float shininess;
	};

public:

	struct TimeBufferType
	{
		Wave waves[6];
		float time;
		XMFLOAT3 padding;
	};

public:
	WaveShader(ID3D11Device* device, HWND hwnd);
	~WaveShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap[3], Light* DirectionalLights[2], Light* PointLights[1], Light* SpotLights[1], Timer* time, Wave* wave[6], Camera* camera);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timeBuffer;
};