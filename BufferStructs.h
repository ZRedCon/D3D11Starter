#pragma once
#include <DirectXMath.h>

struct VertexShaderData {
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct PixelColorBuffer {
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT2 uvScale;
	DirectX::XMFLOAT2 uvOffset;
	float elaspedTime;
};