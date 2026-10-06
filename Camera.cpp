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

}
