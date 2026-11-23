#pragma once
#include <iostream>
#include <memory>
#include "Input.h"
#include "Transform.h"
#include "Window.h"

#include "Inspectable.h"

enum class CameraProjectionMode
{
	Perspective,
	Orthographic
};

class Camera : public Registered<Camera>
{
private:
	static inline std::string currCam = "";
	std::unique_ptr<Transform> transform;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	float aspRatio;
	float fov;
	float nearClip;
	float farClip;
	CameraProjectionMode projection = CameraProjectionMode::Perspective;
public:
	static void UpdateAllCamerasProjMatrix();
	static std::shared_ptr<Camera> GetCurrentCamera();

	Camera(std::string _name, float aspectRatio, XMFLOAT3 initialPosition);


	Transform& GetTransform();
	float GetFov();
	XMFLOAT2 GetClipPlanes();
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	Camera& Orient(XMFLOAT3 orientation);
	Camera& SetFOV(float new_fov);
	Camera& SetClipPlanes(float nearPlane, float farPlane);
	Camera& SetClipPlanes( XMFLOAT2 planes );
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);
	void Update(float dt);

	void Inspector() override;
	static void InspectStatics();
};
