#include "Camera.h"

Camera::Camera(float aspectRatio, XMFLOAT3 initialPosition)
{
	transform = std::make_unique<Transform>();
	fov = XM_PIDIV2;
	nearClip = 0.1f;
	farClip = 1000.0f;
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera& Camera::Orient(XMFLOAT3 orientation)
{
	transform->SetRotation(orientation);
	return *this;
}

Camera& Camera::SetFOV(float new_fov)
{
	fov = new_fov;
	return *this;
}

Camera& Camera::SetClipPlanes(float nearPlane, float farPlane)
{
	nearClip = nearPlane;
	farClip = farPlane;
	return *this;
}

void Camera::UpdateViewMatrix()
{
	XMStoreFloat4x4(&viewMatrix, 
		XMMatrixLookToLH(
			XMLoadFloat3( &(transform->GetPosition()) ),
			XMLoadFloat3(&(transform->GetLocalForward())),
			XMLoadFloat3(&(transform->UP))
		)
	);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip));
}

void Camera::Update(float dt)
{
	static const float MOVE_SPEED = 10;
	XMFLOAT3 planarMovement(0, 0, 0); // X is Z, Y is X
	if (Input::KeyDown('W')) planarMovement.x += +1;
	if (Input::KeyDown('S')) planarMovement.x += -1;
	if (Input::KeyDown('A')) planarMovement.y += +1;
	if (Input::KeyDown('D')) planarMovement.y += -1;
	transform->MoveRelative(planarMovement.y * MOVE_SPEED * dt, 0, planarMovement.x * MOVE_SPEED * dt);

	if (Input::KeyDown('X') || Input::KeyDown(VK_SPACE))
	{
		if (Input::KeyDown('X')) planarMovement.z += -1;
		if (Input::KeyDown(VK_SPACE)) planarMovement.z += +1;
		transform->MoveAbsolute(0, planarMovement.z * MOVE_SPEED * dt, 0);
	}

	if (Input::MouseLeftDown())
	{
		int cursorMovementX = Input::GetMouseXDelta();
		int cursorMovementY = Input::GetMouseYDelta();
		if (cursorMovementX > XM_PIDIV2) cursorMovementX = XM_PIDIV2;
		else if (cursorMovementX < -XM_PIDIV2) cursorMovementX = XM_PIDIV2;
		transform->Rotate(cursorMovementY, cursorMovementX, 0);
	}

	UpdateViewMatrix();
}
