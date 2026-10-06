#include "Transform.h"

const XMFLOAT3 Transform::FORWARD  = XMFLOAT3( 0,  0,  1 );
const XMFLOAT3 Transform::BACKWARD = XMFLOAT3( 0,  0, -1 );
const XMFLOAT3 Transform::LEFT     = XMFLOAT3(-1,  0,  0 );
const XMFLOAT3 Transform::RIGHT    = XMFLOAT3( 1,  0,  0 );
const XMFLOAT3 Transform::UP       = XMFLOAT3( 0,  1,  0 );
const XMFLOAT3 Transform::DOWN     = XMFLOAT3( 0, -1,  0 );

Transform::Transform()
{
	dirty = false;
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);

	localForward = FORWARD;
	localRight = RIGHT;
	localUp = UP;

	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

XMFLOAT3 Transform::GetPosition() { return position; }
XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }
XMFLOAT3 Transform::GetScale() { return scale; }

void Transform::RebuldDirectionVectors()
{
	localForward = FORWARD;
	localRight = RIGHT;
	localUp = UP;

	RotationDirectionByRotation( localForward );
	RotationDirectionByRotation( localRight );
	RotationDirectionByRotation( localUp );
}

void Transform::RotationDirectionByRotation(XMFLOAT3& origin)
{
	auto quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMStoreFloat3(&origin, XMVector3Rotate(XMLoadFloat3(&origin), quat));
}

void Transform::RebuildMatricies()
{
	dirty = false;
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
XMFLOAT3 Transform::GetLocalForward()
{
	return localForward;
}
XMFLOAT3 Transform::GetLocalRight()
{
	return localRight;
}
XMFLOAT3 Transform::GetLocalUp()
{
	return localUp;
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
	RebuldDirectionVectors();
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
	RebuldDirectionVectors();
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
void Transform::MoveRelative(XMFLOAT3 offset)
{
	auto quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMFLOAT3 rotOffset;
	XMStoreFloat3(&rotOffset, XMVector3Rotate(XMLoadFloat3(&offset), quat));
	MoveAbsolute( rotOffset );
}
void Transform::MoveRelative(float x, float y, float z)
{ 
	auto quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMFLOAT3 rotOffset(x,y,z);
	XMStoreFloat3(&rotOffset, XMVector3Rotate(XMLoadFloat3(&rotOffset), quat));
	MoveAbsolute(rotOffset);
}
void Transform::Rotate(XMFLOAT3 newRotation)
{
	dirty = true;
	rotation.x += newRotation.x;
	rotation.y += newRotation.y;
	rotation.z += newRotation.z;
	RebuldDirectionVectors();
}
void Transform::Rotate(float pitch, float yaw, float roll)
{
	dirty = true;
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
	RebuldDirectionVectors();
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
