// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create models and meshes
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	wavemesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	lavamesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphere = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphere1 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	model = new AModel(renderer->getDevice(), "res/teapot.obj");
	boat = new AModel(renderer->getDevice(), "res/boat.obj");

	//Load textures
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	textureMgr->loadTexture(L"volcano", L"res/Volcano.png");

	textureMgr->loadTexture(L"lavarock", L"res/lavarock.jpg");

	textureMgr->loadTexture(L"boat", L"res/boat.png");

	textureMgr->loadTexture(L"water", L"res/water.jpg");

	textureMgr->loadTexture(L"lava", L"res/lava.jpg");

	//initialise shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	manipulationShader = new ManipulationShader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	bloomShader = new BloomShader(renderer->getDevice(), hwnd);
	waveShader = new WaveShader(renderer->getDevice(), hwnd);
	blurShader = new BlurShader(renderer->getDevice(), hwnd);
	depthWaveShader = new DepthWaveShader(renderer->getDevice(), hwnd);
	depthManipulationShader = new DepthManipulationShader(renderer->getDevice(), hwnd);

	
	// Variables for defining shadow map (lower resolution causes strange artifacting)
	int shadowmapWidth = 8000;
	int shadowmapHeight = 8000;


	// Initialise shadowmaps
	for (int i = 0; i < 3; ++i) {
		shadowMaps[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	}


	// Configure directional lights
	DirectionalLights[0] = new Light();
	light1Pos = XMFLOAT3(10.f, 120.f, 150.f);
	light1Direction = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	light1Color = XMFLOAT3(1.0f, 0.85f, 0.0f);
	light1Ambient = XMFLOAT3(0.3f, 0.3f, 0.3f);
	DirectionalLights[0]->generateOrthoMatrix(screenWidth, screenHeight, 0.1f, 200.f);

	DirectionalLights[1] = new Light();
	light2Pos = XMFLOAT3(-10.f, 120.f, 150.f);
	light2Direction = XMFLOAT3(1.0f, -1.0f, 0.0f);
	light2Color = XMFLOAT3(0.125f, 0.0f, 1.0f);
	light2Ambient = XMFLOAT3(0.3f, 0.3f, 0.3f);
	DirectionalLights[1]->generateOrthoMatrix(screenWidth, screenHeight, 0.1f, 200.f);

	//Configure point lights
	PointLights[0] = new Light();
	PointLights[0]->setAmbientColour(1.0f, 1.0f, 1.0f, 1.f);
	PointLights[0]->setDiffuseColour(1.0f, 1.0f, 1.0f, 0.f);
	PointLights[0]->setPosition(4.f, 20.f, 36.f);
	PointLights[0]->generateProjectionMatrix(0.1f, 100.f);

	//Configure spot lights
	SpotLights[0] = new Light();
	spotlightAmbient = XMFLOAT3(0.5f, 0.5f, 0.5f);
	spotlightDiffuse = XMFLOAT3(1.0f, 0.0f, 0.0f);
	SpotLights[0]->setPosition(30.f, 25.f, 65.f);
	SpotLights[0]->setDirection(0.f, -1.f, 0.f);
	SpotLights[0]->generateProjectionMatrix(0.1f, 100.f);

	//Initialise required render textures
	firstRenderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	firstBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth/2, screenHeight/2, SCREEN_NEAR, SCREEN_DEPTH);
	secondBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth/4, screenHeight/4, SCREEN_NEAR, SCREEN_DEPTH);
	thirdBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth/6, screenHeight/6, SCREEN_NEAR, SCREEN_DEPTH);
	firstCombineTexture = new RenderTexture(renderer->getDevice(), screenWidth / 4, screenHeight / 4, SCREEN_NEAR, SCREEN_DEPTH);
	secondCombineTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	finalCombineTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//Initialise orthomeshes
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	firstBlurOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth/2, screenHeight/2);
	secondBlurOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth/4, screenHeight/4);
	thirdBlurOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth/6, screenHeight/6);
	firstCombineOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4);
	secondCombineOrthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 2, screenHeight / 2);

	//Wave configuration
	wave1Direction = XMFLOAT2(1.0f, 1.3f);
	wave2Direction = XMFLOAT2(0.f, 1.f);
	wave3Direction = XMFLOAT2(0.5f, 1.f);
	wave4Direction = XMFLOAT2(1.f, 0.f);
	wave5Direction = XMFLOAT2(1.f, 0.6f);
	wave6Direction = XMFLOAT2(1.f, 1.f);

	wave1Steepness = 0.25f;
	wave2Steepness = 0.25f;
	wave3Steepness = 0.25f;
	wave4Steepness = 0.25f;
	wave5Steepness = 0.25f;
	wave6Steepness = 0.25f;

	wave1Wavelength = 18.f;
	wave2Wavelength = 16.f;
	wave3Wavelength = 14.f;
	wave4Wavelength = 12.f;
	wave5Wavelength = 10.f;
	wave6Wavelength = 8.f;

	waves[0] = new Wave;
	
	waves[1] = new Wave;

	waves[2] = new Wave;

	waves[3] = new Wave;

	waves[4] = new Wave;

	waves[5] = new Wave;

	//initial teapot position (used to be a bird)
	birdPos = XMFLOAT3(115.f, 65.f, 260.f);
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D objects
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}
	if (wavemesh)
	{
		delete wavemesh;
		wavemesh = 0;
	}
	if (lavamesh)
	{
		delete lavamesh;
		lavamesh = 0;
	}
	if (sphere)
	{
		delete sphere;
		sphere = 0;
	}
	if (sphere1)
	{
		delete sphere1;
		sphere1 = 0;
	}
	if (model)
	{
		delete model;
		model = 0;
	}
	if (boat)
	{
		delete boat;
		boat = 0;
	}

	//Release shaders
	if (lightShader)
	{
		delete lightShader;
		lightShader = 0;
	}
	if (manipulationShader)
	{
		delete manipulationShader;
		manipulationShader = 0;
	}
	if (waveShader)
	{
		delete waveShader;
		waveShader = 0;
	}
	if (bloomShader)
	{
		delete bloomShader;
		bloomShader = 0;
	}
	if (blurShader)
	{
		delete blurShader;
		blurShader = 0;
	}
	if (textureShader)
	{
		delete textureShader;
		textureShader = 0;
	}
	if (depthShader) 
	{
		delete depthShader;
		depthShader = 0;
	}
	if (depthManipulationShader)
	{
		delete depthManipulationShader;
		depthManipulationShader = 0;
	}
	if (depthWaveShader)
	{
		delete depthWaveShader;
		depthWaveShader = 0;
	}


}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	//Update light settings
	DirectionalLights[0]->setAmbientColour(light1Ambient.x, light1Ambient.y, light1Ambient.z, 1.0f);
	DirectionalLights[0]->setDiffuseColour(light1Color.x, light1Color.y, light1Color.z, 1.0f);
	DirectionalLights[0]->setDirection(light1Direction.x, light1Direction.y, light1Direction.z);
	DirectionalLights[0]->setPosition(light1Pos.x, light1Pos.y, light1Pos.z);

	DirectionalLights[1]->setAmbientColour(light2Ambient.x, light2Ambient.y, light2Ambient.z, 1.0f);
	DirectionalLights[1]->setDiffuseColour(light2Color.x, light2Color.y, light2Color.z, 1.0f);
	DirectionalLights[1]->setDirection(light2Direction.x, light2Direction.y, light2Direction.z);
	DirectionalLights[1]->setPosition(light2Pos.x, light2Pos.y, light2Pos.z);

	SpotLights[0]->setDiffuseColour(spotlightDiffuse.x, spotlightDiffuse.y, spotlightDiffuse.z, 1.f);
	SpotLights[0]->setAmbientColour(spotlightAmbient.x, spotlightAmbient.y, spotlightAmbient.z, 1.f);

	//Update wave settings
	waves[0]->direction = wave1Direction;
	waves[0]->steepness = wave1Steepness;
	waves[0]->wavelength = wave1Wavelength;

	waves[1]->direction = wave2Direction;
	waves[1]->steepness = wave2Steepness;
	waves[1]->wavelength = wave2Wavelength;

	waves[2]->direction = wave3Direction;
	waves[2]->steepness = wave3Steepness;
	waves[2]->wavelength = wave3Wavelength;

	waves[3]->direction = wave4Direction;
	waves[3]->steepness = wave4Steepness;
	waves[3]->wavelength = wave4Wavelength;

	waves[4]->direction = wave5Direction;
	waves[4]->steepness = wave5Steepness;
	waves[4]->wavelength = wave5Wavelength;

	waves[5]->direction = wave6Direction;
	waves[5]->steepness = wave6Steepness;
	waves[5]->wavelength = wave6Wavelength;


	// Perform depth pass
	depthPass();

	if (bloomThreshold != 0 || bloomStrength != 0) 
	{
		//Do post processing if it is turned on
		lightRenderPass();
		bloomPass();
		firstBlurPass();
		secondBlurPass();
		thirdBlurPass();
		firstCombinePass();
		secondCombinePass();
		finalCombinePass();
	}

	// Render scene
	finalPass();


	return true;
}

void App1::depthPass()
{
	// Iterate through each directional light
	for (int i = 0; i < 2; ++i) {
		shadowMaps[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

		// get the world, view, and projection matrices from the camera and d3d objects.
		DirectionalLights[i]->generateViewMatrix();
		XMMATRIX lightViewMatrix = DirectionalLights[i]->getViewMatrix();
		XMMATRIX lightProjectionMatrix = DirectionalLights[i]->getOrthoMatrix();
		XMMATRIX worldMatrix = renderer->getWorldMatrix();

		// Render floor
		worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
		mesh->sendData(renderer->getDeviceContext());
		depthManipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"volcano"), nullptr, DirectionalLights, PointLights, SpotLights, camera);
		depthManipulationShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
		//Render wave
		worldMatrix = XMMatrixTranslation(-50.f, 4.f, -10.f);
		wavemesh->sendData(renderer->getDeviceContext());
		depthWaveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix,
			textureMgr->getTexture(L"water"), nullptr, DirectionalLights, PointLights, SpotLights, timer, waves, camera);
		depthWaveShader->render(renderer->getDeviceContext(), wavemesh->getIndexCount());
		//Render lava
		worldMatrix = XMMatrixTranslation(-40.f, 40.f, 78.f);
		XMMATRIX scaleMatrix0 = XMMatrixScaling(0.3f, 0.3f, 0.3f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix0);
		lavamesh->sendData(renderer->getDeviceContext());
		depthWaveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix,
			textureMgr->getTexture(L"water"), nullptr, DirectionalLights, PointLights, SpotLights, timer, waves, camera);
		depthWaveShader->render(renderer->getDeviceContext(), lavamesh->getIndexCount());
		// Render model
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(birdPos.x, birdPos.y, birdPos.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		model->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), model->getIndexCount());
		//Render boat
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(-40.f, 5.f, 20.f);
		boat->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), boat->getIndexCount());

		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}

	//Iterate through spot light
	shadowMaps[2]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	SpotLights[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = SpotLights[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = SpotLights[0]->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// Render floor
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext());
	depthManipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"volcano"), nullptr, DirectionalLights, PointLights, SpotLights, camera);
	depthManipulationShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	//Render waves
	worldMatrix = XMMatrixTranslation(-50.f, 4.f, -10.f);
	wavemesh->sendData(renderer->getDeviceContext());
	depthWaveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix,
		textureMgr->getTexture(L"water"), nullptr, DirectionalLights, PointLights, SpotLights, timer, waves, camera);
	depthWaveShader->render(renderer->getDeviceContext(), wavemesh->getIndexCount());
	//Render lava
	worldMatrix = XMMatrixTranslation(-40.f, 40.f, 78.f);
	XMMATRIX scaleMatrix0 = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix0);
	lavamesh->sendData(renderer->getDeviceContext());
	depthWaveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix,
		textureMgr->getTexture(L"lava"), nullptr, DirectionalLights, PointLights, SpotLights, timer, waves, camera);
	depthWaveShader->render(renderer->getDeviceContext(), lavamesh->getIndexCount());

	// Render model
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(birdPos.x, birdPos.y, birdPos.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());
	//Render boat
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-40.f, 5.f, 20.f);
	boat->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), boat->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::lightRenderPass()
{
	//This pass renders the scene with lights and shadows onto a render texture
	firstRenderTexture->setRenderTarget(renderer->getDeviceContext());
	firstRenderTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	ID3D11ShaderResourceView* shadowMapSRVs[3] = { shadowMaps[0]->getDepthMapSRV(), shadowMaps[1]->getDepthMapSRV(), shadowMaps[2]->getDepthMapSRV()};

	// Render floor
	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	mesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	manipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"volcano"), textureMgr->getTexture(L"lavarock"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, camera);
	manipulationShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
	//Render waves
	worldMatrix = XMMatrixTranslation(-50.f, 4.f, -10.f);
	wavemesh->sendData(renderer->getDeviceContext());
	waveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"water"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, timer, waves, camera);
	waveShader->render(renderer->getDeviceContext(), wavemesh->getIndexCount());
	//Render lava
	worldMatrix = XMMatrixTranslation(-40.f, 40.f, 78.f);
	XMMATRIX scaleMatrix0 = XMMatrixScaling(0.3f, 0.3f, 0.3f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix0);
	lavamesh->sendData(renderer->getDeviceContext());
	waveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"lava"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, timer, waves, camera);
	waveShader->render(renderer->getDeviceContext(), lavamesh->getIndexCount());
	// Render model
	worldMatrix = XMMatrixTranslation(birdPos.x, birdPos.y, birdPos.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	model->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
	 textureMgr->getTexture(L"brick"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, camera);
	lightShader->render(renderer->getDeviceContext(), model->getIndexCount());
	//Render boat
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(-40.f, 5.f, 20.f);
	boat->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"boat"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, camera);
	lightShader->render(renderer->getDeviceContext(), boat->getIndexCount());

	renderer->setBackBufferRenderTarget();
}

void App1::bloomPass()
{
	//This pass extracts the bright pixels onto a render texture to prepare them for blur
	bloomTexture->setRenderTarget(renderer->getDeviceContext());
	bloomTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();

	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	bloomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, firstRenderTexture->getShaderResourceView(), bloomStrength, bloomThreshold);
	bloomShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::firstBlurPass()
{
	//This pass blurs the bloom texture
	firstBlurTexture->setRenderTarget(renderer->getDeviceContext());
	firstBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = firstBlurTexture->getOrthoMatrix();

	float screenSizeY = (float)firstBlurTexture->getTextureHeight();
	float screenSizeX = (float)firstBlurTexture->getTextureWidth();

	renderer->setZBuffer(false);
	firstBlurOrthoMesh->sendData(renderer->getDeviceContext());
	blurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, bloomTexture->getShaderResourceView(), screenSizeX, screenSizeY);
	blurShader->render(renderer->getDeviceContext(), firstBlurOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::secondBlurPass()
{
	//This pass blurs the blur again onto a lower resolution
	secondBlurTexture->setRenderTarget(renderer->getDeviceContext());
	secondBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = secondBlurTexture->getOrthoMatrix();

	float screenSizeY = (float)secondBlurTexture->getTextureHeight();
	float screenSizeX = (float)secondBlurTexture->getTextureWidth();

	renderer->setZBuffer(false);
	secondBlurOrthoMesh->sendData(renderer->getDeviceContext());
	blurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, firstBlurTexture->getShaderResourceView(), screenSizeX, screenSizeY);
	blurShader->render(renderer->getDeviceContext(), secondBlurOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::thirdBlurPass()
{
	//Blurs again at a lower resolution
	thirdBlurTexture->setRenderTarget(renderer->getDeviceContext());
	thirdBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = thirdBlurTexture->getOrthoMatrix();

	float screenSizeY = (float)thirdBlurTexture->getTextureHeight();
	float screenSizeX = (float)thirdBlurTexture->getTextureWidth();

	renderer->setZBuffer(false);
	thirdBlurOrthoMesh->sendData(renderer->getDeviceContext());
	blurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, secondBlurTexture->getShaderResourceView(), screenSizeX, screenSizeY);
	blurShader->render(renderer->getDeviceContext(), thirdBlurOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::firstCombinePass()
{
	//Combine the lowest and second lowest resolution blurs onto an upscaled render texture
	firstCombineTexture->setRenderTarget(renderer->getDeviceContext());
	firstCombineTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = firstCombineTexture->getOrthoMatrix();

	renderer->setZBuffer(false);
	firstCombineOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, thirdBlurTexture->getShaderResourceView(), secondBlurTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), firstCombineOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::secondCombinePass()
{
	//Combine the second highest blur with the highest resolution blur texture onto another upscaled render texture
	secondCombineTexture->setRenderTarget(renderer->getDeviceContext());
	secondCombineTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = secondCombineTexture->getOrthoMatrix();

	renderer->setZBuffer(false);
	secondCombineOrthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, secondBlurTexture->getShaderResourceView(), firstBlurTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), secondCombineOrthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::finalCombinePass()
{
	//Combine the two upscaled render textures
	finalCombineTexture->setRenderTarget(renderer->getDeviceContext());
	finalCombineTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();

	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, firstCombineTexture->getShaderResourceView(), secondCombineTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

void App1::finalPass()
{

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	if (bloomThreshold != 0 || bloomStrength != 0) 
	{
		//If post processing is enabled, render the scene as the final combined blur textures combined with the original render texture
		XMMATRIX worldMatrix = renderer->getWorldMatrix();
		XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
		XMMATRIX orthoMatrix = renderer->getOrthoMatrix();

		renderer->setZBuffer(false);
		orthoMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, firstRenderTexture->getShaderResourceView(), finalCombineTexture->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
		renderer->setZBuffer(true);

		renderer->setBackBufferRenderTarget();
	}
	else
	{
		//If post processing is enabled, render the scene as normal for debugging purposes, including two spheres to show the positions of the directional lights

		// get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		XMMATRIX worldMatrix = renderer->getWorldMatrix();
		XMMATRIX viewMatrix = camera->getViewMatrix();
		XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

		ID3D11ShaderResourceView* shadowMapSRVs[9] = { shadowMaps[0]->getDepthMapSRV(), shadowMaps[1]->getDepthMapSRV(), shadowMaps[2]->getDepthMapSRV()};

		// Render floor
		worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
		mesh->sendData(renderer->getDeviceContext(), D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		manipulationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"volcano"), textureMgr->getTexture(L"lavarock"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, camera);
		manipulationShader->render(renderer->getDeviceContext(), mesh->getIndexCount());
		//Render waves
		worldMatrix = XMMatrixTranslation(-50.f, 4.f, -10.f);
		wavemesh->sendData(renderer->getDeviceContext());
		waveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"water"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, timer, waves, camera);
		waveShader->render(renderer->getDeviceContext(), wavemesh->getIndexCount());
		//Render lava
		worldMatrix = XMMatrixTranslation(-40.f, 40.f, 78.f);
		XMMATRIX scaleMatrix0 = XMMatrixScaling(0.3f, 0.3f, 0.3f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix0);
		lavamesh->sendData(renderer->getDeviceContext());
		waveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"lava"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, timer, waves, camera);
		waveShader->render(renderer->getDeviceContext(), lavamesh->getIndexCount());

		// Render model
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(birdPos.x, birdPos.y, birdPos.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		model->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"brick"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, camera);
		lightShader->render(renderer->getDeviceContext(), model->getIndexCount());
		//Render boat
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(-40.f, 5.f, 20.f);
		boat->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"boat"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, camera);
		lightShader->render(renderer->getDeviceContext(), boat->getIndexCount());
		//Render first light sphere
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(light1Pos.x, light1Pos.y, light1Pos.z);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		sphere->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"brick"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, camera);
		lightShader->render(renderer->getDeviceContext(), sphere->getIndexCount());
		//Render second light sphere
		worldMatrix = renderer->getWorldMatrix();
		worldMatrix = XMMatrixTranslation(light2Pos.x, light2Pos.y, light2Pos.z);
		worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
		sphere1->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
			textureMgr->getTexture(L"brick"), shadowMapSRVs, DirectionalLights, PointLights, SpotLights, camera);
		lightShader->render(renderer->getDeviceContext(), sphere1->getIndexCount());

		renderer->setBackBufferRenderTarget();
	}


	gui();
	renderer->endScene();
}


void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	//Bloom config
	if (ImGui::CollapsingHeader("Bloom Settings"))
	{
		ImGui::SliderFloat("Bloom Threshold", &bloomThreshold, 0.0f, 1.0f);
		ImGui::SliderFloat("Bloom Strength", &bloomStrength, 0.0f, 3.0f);
	}
	//Teapot position config
	if(ImGui::CollapsingHeader("Teapot Position"))
	{
		ImGui::SliderFloat3("Position", &birdPos.x, -300, 300);
	}
	//Wave config
	if (ImGui::CollapsingHeader("Wave Settings"))
	{
		if(ImGui::CollapsingHeader("Wave 1 Settings"))
		{
			ImGui::SliderFloat("Wave 1 Wavelength", &wave1Wavelength, 0.01f, 20.0f);
			ImGui::SliderFloat("Wave 1 Steepness", &wave1Steepness, 0.0f, 1.0f);
			ImGui::SliderFloat2("Wave 1 Direction", &wave1Direction.x, -2.0f, 2.0f);
		}
		if (ImGui::CollapsingHeader("Wave 2 Settings"))
		{
			ImGui::SliderFloat("Wave 2 Wavelength", &wave2Wavelength, 0.01f, 20.0f);
			ImGui::SliderFloat("Wave 2 Steepness", &wave2Steepness, 0.0f, 1.0f);
			ImGui::SliderFloat2("Wave 2 Direction", &wave2Direction.x, -2.0f, 2.0f);

		}
		if (ImGui::CollapsingHeader("Wave 3 Settings"))
		{
			ImGui::SliderFloat("Wave 3 Wavelength", &wave3Wavelength, 0.01f, 20.0f);
			ImGui::SliderFloat("Wave 3 Steepness", &wave3Steepness, 0.0f, 1.0f);
			ImGui::SliderFloat2("Wave 3 Direction", &wave3Direction.x, -2.0f, 2.0f);

		}
		if (ImGui::CollapsingHeader("Wave 4 Settings"))
		{
			ImGui::SliderFloat("Wave 4 Wavelength", &wave4Wavelength, 0.01f, 20.0f);
			ImGui::SliderFloat("Wave 4 Steepness", &wave4Steepness, 0.0f, 1.0f);
			ImGui::SliderFloat2("Wave 4 Direction", &wave4Direction.x, -2.0f, 2.0f);

		}
		if (ImGui::CollapsingHeader("Wave 5 Settings"))
		{
			ImGui::SliderFloat("Wave 5 Wavelength", &wave5Wavelength, 0.01f, 20.0f);
			ImGui::SliderFloat("Wave 5 Steepness", &wave5Steepness, 0.0f, 1.0f);
			ImGui::SliderFloat2("Wave 5 Direction", &wave5Direction.x, -2.0f, 2.0f);

		}
		if (ImGui::CollapsingHeader("Wave 6 Settings"))
		{
			ImGui::SliderFloat("Wave 6 Wavelength", &wave6Wavelength, 0.01f, 20.0f);
			ImGui::SliderFloat("Wave 6 Steepness", &wave6Steepness, 0.0f, 1.0f);
			ImGui::SliderFloat2("Wave 6 Direction", &wave6Direction.x, -2.0f, 2.0f);

		}
	}
	//Directional light config
	if (ImGui::CollapsingHeader("Directional Light Settings")) 
	{
		if(ImGui::CollapsingHeader("Light 1 Settings"))
		{
			ImGui::SliderFloat3("Light 1 Direction", &light1Direction.x, -1.0f, 1.0f);
			ImGui::SliderFloat3("Light 1 Position", &light1Pos.x, -200.f, 200.f);
			ImGui::SliderFloat3("Light 1 Color", &light1Color.x, 0.0f, 1.0f);
			ImGui::SliderFloat3("Light 1 Ambient Color", &light1Ambient.x, 0.0f, 1.0f);
		}
		if (ImGui::CollapsingHeader("Light 2 Settings"))
		{
			ImGui::SliderFloat3("Light 2 Direction", &light2Direction.x, -1.0f, 1.0f);
			ImGui::SliderFloat3("Light 2 Position", &light2Pos.x, -200.f, 200.f);
			ImGui::SliderFloat3("Light 2 Color", &light2Color.x, 0.0f, 1.0f);
			ImGui::SliderFloat3("Light 2 Ambient Color", &light2Ambient.x, 0.0f, 1.0f);
		}
	}
	//Spotlight color config
	if (ImGui::CollapsingHeader("Spotlight Color Settings")) 
	{
		ImGui::SliderFloat3("Spotlight Ambient", &spotlightAmbient.x, 0.0f, 1.0f);
		ImGui::SliderFloat3("Spotlight Diffuse", &spotlightDiffuse.x, 0.0f, 1.0f);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}