#include "Transform.h"

Transform::Transform()
{
	dirty = false;
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

XMFLOAT3 Transform::GetPosition() { return position; }
XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }
XMFLOAT3 Transform::GetScale() { return scale; }

void Transform::RebuildMatricies()
{
	auto translation = XMMatrixTranslation(position.x, position.y, position.z);
	auto newScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	auto newRotation = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX newWorldMatrix = newScale * newRotation * translation;
	XMStoreFloat4x4(&world, newWorldMatrix);
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixTranspose( newWorldMatrix ));
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty) RebuildMatricies();
	return world;
}
XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (dirty) RebuildMatricies();
	return worldInverseTranspose;
}
void Transform::SetPosition(XMFLOAT3 newPosition)
{
	//if (newPosition.x == position.x && newPosition.y == position.y && newPosition.z == position.z) return;
	dirty = true;
	position = newPosition;
}
void Transform::SetRotation(XMFLOAT3 newRotation)
{
	dirty = true;
	rotation = newRotation;
}
void Transform::SetScale(XMFLOAT3 newScale)
{
	dirty = true;
	scale = newScale;
}

void Transform::SetPosition(float x, float y, float z)
{
	dirty = true;
	position = XMFLOAT3(x, y, z);
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	dirty = true;
	rotation = XMFLOAT3(pitch, yaw, roll);
}

void Transform::SetScale(float x, float y, float z)
{
	dirty = true;
	scale = XMFLOAT3(x, y, z);
}

void Transform::MoveAbsolute(XMFLOAT3 newPosition)
{
	dirty = true;
	position.x += newPosition.x;
	position.y += newPosition.y;
	position.z += newPosition.z;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	dirty = true;
	position.x += x;
	position.y += y;
	position.z += z;
}

void Transform::Rotate(XMFLOAT3 newRotation)
{
	dirty = true;
	rotation.x += newRotation.x;
	rotation.y += newRotation.y;
	rotation.z += newRotation.z;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	dirty = true;
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
}

void Transform::Scale(XMFLOAT3 newScale)
{
	dirty = true;
	scale.x *= newScale.x;
	scale.y *= newScale.y;
	scale.z *= newScale.z;
}

void Transform::Scale(float x, float y, float z)
{
	dirty = true;
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}
