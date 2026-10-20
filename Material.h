#pragma once
#include <DirectXMath.h>
#include <array>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <cstddef>
#include <memory>
#include <iostream>

#include "Graphics.h"
#include "Vertex.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Material
{
private:
	XMFLOAT4 tint;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
public:
	Material(XMFLOAT4 startTint, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);
	Material(XMFLOAT3 startTint, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);
	Material(ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);

	XMFLOAT4 GetTint() const;
	ComPtr<ID3D11VertexShader> GetVertexShader() const;
	ComPtr<ID3D11PixelShader> GetPixelShader() const;

	void SetTint(XMFLOAT4 newTint);
	void SetVertexShader(ComPtr<ID3D11VertexShader> newVertexShader);
	void SetPixelShader(ComPtr<ID3D11PixelShader> newPixelShader);
};

