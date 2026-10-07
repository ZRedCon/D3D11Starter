#include "Camera.h"
#include <iostream>

Camera::Camera(float aspectRatio, XMFLOAT3 initialPosition)
{
	transform = std::make_unique<Transform>();
	transform->SetPosition( initialPosition );
	aspRatio = aspectRatio;
	fov = XM_PIDIV2;
	nearClip = 0.1f;
	farClip = 1000.0f;
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspRatio);
}

Transform& Camera::GetTransform()
{
	return *transform;
}

float Camera::GetFov()
{
	return fov;
}

XMFLOAT2 Camera::GetClipPlanes()
{
	return XMFLOAT2(nearClip, farClip);
}


XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

Camera& Camera::Orient(XMFLOAT3 orientation)
{
	transform->SetRotation(orientation);
	return *this;
}

Camera& Camera::SetFOV(float new_fov)
{
	fov = new_fov;
	UpdateProjectionMatrix(aspRatio);
	return *this;
}

Camera& Camera::SetClipPlanes(float nearPlane, float farPlane)
{
	nearClip = nearPlane;
	farClip = farPlane;
	UpdateProjectionMatrix(aspRatio);
	return *this;
}

Camera& Camera::SetClipPlanes(XMFLOAT2 planes)
{
	SetClipPlanes(planes.x, planes.y);
	return *this;
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 pos = transform->GetPosition();
	XMFLOAT3 forward = transform->GetLocalForward();
	XMFLOAT3 up = transform->UP;

	XMStoreFloat4x4(&viewMatrix, 
		XMMatrixLookToLH(
			XMLoadFloat3( &pos ), XMLoadFloat3( &forward ), XMLoadFloat3( &up )
		)
	);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	aspRatio = aspectRatio;
	XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(fov, aspRatio, nearClip, farClip));
}

void Camera::Update(float dt)
{
	static const float MOVE_SPEED = 10;
	XMFLOAT3 planarMovement(0, 0, 0); // X is Z, Y is X
	if (Input::KeyDown('W')) planarMovement.x += +1;
	if (Input::KeyDown('S')) planarMovement.x += -1;
	if (Input::KeyDown('A')) planarMovement.y += -1;
	if (Input::KeyDown('D')) planarMovement.y += +1;
	transform->MoveRelative(planarMovement.y * MOVE_SPEED * dt, 0, planarMovement.x * MOVE_SPEED * dt);

	if (Input::KeyDown('X') || Input::KeyDown(VK_SPACE))
	{
		if (Input::KeyDown('X')) planarMovement.z += -1;
		if (Input::KeyDown(VK_SPACE)) planarMovement.z += +1;
		transform->MoveAbsolute(0, planarMovement.z * MOVE_SPEED * dt, 0);
	}

	if (Input::MouseLeftDown())
	{
		float cursorMovementX = Input::GetMouseXDelta();
		int cursorMovementY = Input::GetMouseYDelta();
		if (cursorMovementX > XM_PIDIV2) cursorMovementX = XM_PIDIV2;
		else if (cursorMovementX < -XM_PIDIV2) cursorMovementX = XM_PIDIV2;
		transform->Rotate(cursorMovementY * 0.05f, cursorMovementX * 0.05f, 0);
	}

	UpdateViewMatrix();
}
