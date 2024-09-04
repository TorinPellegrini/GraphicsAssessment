// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "DepthShader.h"
#include "ManipulationShader.h"
#include "LightShader.h"
#include "BloomShader.h"
#include "WaveShader.h"
#include "BlurShader.h"
#include "DepthWaveShader.h"
#include "DepthManipulationShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass();
	void lightRenderPass();
	void bloomPass();
	void firstBlurPass();
	void secondBlurPass();
	void thirdBlurPass();
	void firstCombinePass();
	void secondCombinePass();
	void finalCombinePass();
	void finalPass();
	void gui();

private:
	//Geometry variables
	PlaneMesh* mesh;
	PlaneMesh* wavemesh;
	PlaneMesh* lavamesh;
	AModel* model;
	AModel* boat;
	AModel* bird;
	AModel* helicopter;
	SphereMesh* sphere;
	SphereMesh* sphere1;

	//Lights
	Light* DirectionalLights[2];
	Light* PointLights[1];
	Light* SpotLights[1];

	//Shaders
	TextureShader* textureShader;
	DepthShader* depthShader;
	ManipulationShader* manipulationShader;
	LightShader* lightShader;
	BloomShader* bloomShader;
	WaveShader* waveShader;
	BlurShader* blurShader;
	DepthWaveShader* depthWaveShader;
	DepthManipulationShader* depthManipulationShader;

	//Orthomeshes
	OrthoMesh* orthoMesh;
	OrthoMesh* firstBlurOrthoMesh;
	OrthoMesh* secondBlurOrthoMesh;
	OrthoMesh* thirdBlurOrthoMesh;
	OrthoMesh* firstCombineOrthoMesh;
	OrthoMesh* secondCombineOrthoMesh;

	//Render Textures
	RenderTexture* firstRenderTexture;
	RenderTexture* bloomTexture;
	RenderTexture* firstBlurTexture;
	RenderTexture* secondBlurTexture;
	RenderTexture* thirdBlurTexture;
	RenderTexture* firstCombineTexture;
	RenderTexture* secondCombineTexture;
	RenderTexture* finalCombineTexture;

	//Shadow maps
	ShadowMap* shadowMaps[3];

	//Waves
	Wave* waves[6];

	//Bloom settings
	float bloomThreshold;
	float bloomStrength;

	//Wave variables
	XMFLOAT2 wave1Direction;
	XMFLOAT2 wave2Direction;
	XMFLOAT2 wave3Direction;
	XMFLOAT2 wave4Direction;
	XMFLOAT2 wave5Direction;
	XMFLOAT2 wave6Direction;

	float wave1Steepness;
	float wave2Steepness;
	float wave3Steepness;
	float wave4Steepness;
	float wave5Steepness;
	float wave6Steepness;

	float wave1Wavelength;
	float wave2Wavelength;
	float wave3Wavelength;
	float wave4Wavelength;
	float wave5Wavelength;
	float wave6Wavelength;

	//Teapot position
	XMFLOAT3 birdPos;

	//Light variables for sliders
	XMFLOAT3 light1Pos;
	XMFLOAT3 light1Direction;
	XMFLOAT3 light1Color;
	XMFLOAT3 light1Ambient;

	XMFLOAT3 light2Pos;
	XMFLOAT3 light2Direction;
	XMFLOAT3 light2Color;
	XMFLOAT3 light2Ambient;

	XMFLOAT3 spotlightAmbient;
	XMFLOAT3 spotlightDiffuse;

};

#endif