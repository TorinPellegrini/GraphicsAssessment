//Bloom shader
//This shader passes a texture to a pixel shader, which extracts the pixels over a certain brightness threshold
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class BloomShader : public BaseShader
{
private:
	struct BloomBufferType
	{
		float strength;
		float threshold;
		XMFLOAT2 padding;
	};
public:

	BloomShader(ID3D11Device* device, HWND hwnd);
	~BloomShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float strength, float threshold);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* bloomBuffer;
};
