#pragma once
#include <memory>
#include "Input.h"
#include "Transform.h"

enum class CameraProjectionMode
{
	Perspective,
	Orthographic
};

class Camera
{
private:
	std::unique_ptr<Transform> transform;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	float fov;
	float nearClip;
	float farClip;
	CameraProjectionMode projection = CameraProjectionMode::Perspective;
public:
	Camera(float aspectRatio, XMFLOAT3 initialPosition);
	Camera& Orient(XMFLOAT3 orientation);
	Camera& SetFOV(float new_fov);
	Camera& SetClipPlanes(float nearPlane, float farPlane);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);
	void Update(float dt);
};
