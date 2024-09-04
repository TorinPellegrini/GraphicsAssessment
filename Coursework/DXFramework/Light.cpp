// Light class
// Holds data that represents a single light source
#include "light.h"

// create view matrix, based on light position and lookat. Used for shadow mapping.
void Light::generateViewMatrix()
{
	// default up vector
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	if (direction.y == 1 || (direction.x == 0 && direction.z == 0))
	{
		up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0);
	}
	else if (direction.y == -1 || (direction.x == 0 && direction.z == 0))
	{
		up = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0);
	}
	//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR dir = XMVectorSet(direction.x, direction.y, direction.z, 1.0f);
	XMVECTOR right = XMVector3Cross(dir, up);
	up = XMVector3Cross(right, dir);
	// Create the view matrix from the three vectors.
	viewMatrix = XMMatrixLookAtLH(position, position + dir, up);
}

void Light::generateCubeViewMatrix(int faceIndex)
{
	// Directions for each face of the cube map
	static const XMVECTORF32 lookAt[6] = {
		{1.0f, 0.0f, 0.0f},   // +X
		{-1.0f, 0.0f, 0.0f},  // -X
		{0.0f, 1.0f, 0.0f},   // +Y
		{0.0f, -1.0f, 0.0f},  // -Y
		{0.0f, 0.0f, 1.0f},   // +Z
		{0.0f, 0.0f, -1.0f}   // -Z
	};

	// Calculate the right and up vectors
	XMVECTOR right = XMVector3Cross(lookAt[faceIndex], XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMVECTOR up = XMVector3Cross(right, lookAt[faceIndex]);

	// Ensure the up vector is not zero
	if (XMVector3Equal(up, XMVectorZero()))
	{
		// If up vector is zero, use a default up vector (e.g., world up vector)
		up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	}

	// Normalize the vectors
	right = XMVector3Normalize(right);
	up = XMVector3Normalize(up);

	// Create the view matrix from the specified face direction and up vector.
	cubeViewMatrix = XMMatrixLookAtLH(position, position + lookAt[faceIndex], up);
}

// Create a projection matrix for the (point) light source. Used in shadow mapping.
void Light::generateProjectionMatrix(float screenNear, float screenFar)
{
	float fieldOfView, screenAspect;

	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenFar);
}

// Create orthomatrix for (directional) light source. Used in shadow mapping.
void Light::generateOrthoMatrix(float screenWidth, float screenHeight, float near, float far)
{
	orthoMatrix = XMMatrixOrthographicLH(screenWidth, screenHeight, near, far);
}

void Light::setAmbientColour(float red, float green, float blue, float alpha)
{
	ambientColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::setDiffuseColour(float red, float green, float blue, float alpha)
{
	diffuseColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::setDirection(float x, float y, float z)
{
	direction = XMFLOAT3(x, y, z);
}

void Light::setSpecularColour(float red, float green, float blue, float alpha)
{
	specularColour = XMFLOAT4(red, green, blue, alpha);
}

void Light::setSpecularPower(float power)
{
	specularPower = power;
}

void Light::setPosition(float x, float y, float z)
{
	position = XMVectorSet(x, y, z, 1.0f);
}

XMFLOAT4 Light::getAmbientColour()
{
	return ambientColour;
}

XMFLOAT4 Light::getDiffuseColour()
{
	return diffuseColour;
}


XMFLOAT3 Light::getDirection()
{
	return direction;
}

XMFLOAT4 Light::getSpecularColour()
{
	return specularColour;
}


float Light::getSpecularPower()
{
	return specularPower;
}

XMFLOAT3 Light::getPosition()
{
	XMFLOAT3 temp(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
	return temp;
}

void Light::setLookAt(float x, float y, float z)
{
	lookAt = XMVectorSet(x, y, z, 1.0f);
}

XMMATRIX Light::getViewMatrix()
{
	return viewMatrix;
}

XMMATRIX Light::getCubeViewMatrix()
{
	return cubeViewMatrix;
}

XMMATRIX Light::getProjectionMatrix()
{
	return projectionMatrix;
}

XMMATRIX Light::getOrthoMatrix()
{
	return orthoMatrix;
}