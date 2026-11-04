#pragma once
#include <DirectXMath.h>
#include "Lights.h"

struct VertexShaderData {
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInvTranspose;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct PixelColorBuffer {
	Light light;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	DirectX::XMFLOAT3 cameraPosition;
	float elaspedTime;
	DirectX::XMFLOAT3 ambiantColor;
};