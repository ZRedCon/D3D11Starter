#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Transform
{
private:
	bool dirty;
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	XMFLOAT4X4 world;
	XMFLOAT4X4 worldInverseTranspose;

	XMFLOAT3 localForward;
	XMFLOAT3 localRight;
	XMFLOAT3 localUp;

	void RebuildMatricies();
	void RebuldDirectionVectors();
	void RotationDirectionByRotation(XMFLOAT3& origin);
public:
	static const XMFLOAT3 FORWARD;
	static const XMFLOAT3 BACKWARD;
	static const XMFLOAT3 LEFT;
	static const XMFLOAT3 RIGHT;
	static const XMFLOAT3 UP;
	static const XMFLOAT3 DOWN;
	Transform();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetPitchYawRoll();
	XMFLOAT3 GetScale();
	XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	XMFLOAT4X4 GetWorldMatrix();

	XMFLOAT3 GetLocalForward();
	XMFLOAT3 GetLocalRight();
	XMFLOAT3 GetLocalUp();

	void SetPosition(XMFLOAT3 newPosition);
	void SetPosition(float x, float y, float z);
	void SetRotation(XMFLOAT3 newRotation);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(XMFLOAT3 newScale);
	void SetScale(float x, float y, float z);

	void MoveAbsolute(XMFLOAT3 newPosition);
	void MoveAbsolute(float x, float y, float z);
	void MoveRelative(XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z);
	void Rotate(XMFLOAT3 newRotation);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(XMFLOAT3 newScale);
	void Scale(float x, float y, float z);
};
