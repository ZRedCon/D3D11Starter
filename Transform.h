#pragma once
#include <DirectXMath.h>
#include <string>

#include "Inspectable.h"

using namespace DirectX;

class Transform : public Inspectable
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
	Transform(std::string _name);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetPitchYawRoll();
	XMFLOAT3 GetScale();
	XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	XMFLOAT4X4 GetWorldMatrix();

	XMFLOAT3 GetLocalForward();
	XMFLOAT3 GetLocalRight();
	XMFLOAT3 GetLocalUp();

	Transform& SetPosition(XMFLOAT3 newPosition);
	Transform& SetPosition(float x, float y, float z);
	Transform& SetRotation(XMFLOAT3 newRotation);
	Transform& SetRotation(float pitch, float yaw, float roll);
	Transform& SetScale(XMFLOAT3 newScale);
	Transform& SetScale(float x, float y, float z);

	Transform& MoveAbsolute(XMFLOAT3 newPosition);
	Transform& MoveAbsolute(float x, float y, float z);
	Transform& MoveRelative(XMFLOAT3 offset);
	Transform& MoveRelative(float x, float y, float z);
	Transform& Rotate(XMFLOAT3 newRotation);
	Transform& Rotate(float pitch, float yaw, float roll);
	Transform& Scale(XMFLOAT3 newScale);
	Transform& Scale(float x, float y, float z);

	void Inspector() override;
};
