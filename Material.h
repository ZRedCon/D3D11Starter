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

	std::array<ComPtr<ID3D11ShaderResourceView>, 128> textureSRVs{};
	std::array<ComPtr<ID3D11SamplerState>, 16> samplers{};

	XMFLOAT2 uvScale = XMFLOAT2(1, 1);
	XMFLOAT2 uvOffset = XMFLOAT2(0, 0);
public:
	Material(XMFLOAT4 startTint, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);
	Material(XMFLOAT3 startTint, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);
	Material(ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);
	
	void AddTextureSRVAndSampler(unsigned int slot, ComPtr<ID3D11ShaderResourceView> srv, ComPtr<ID3D11SamplerState> sampler);
	void AddTextureSRV(unsigned int slot, ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(unsigned int slot, ComPtr<ID3D11SamplerState> sampler);

	void BindTexturesAndSamplers();

	XMFLOAT2 GetUVScale() const;
	XMFLOAT2 GetUVOffset() const;
	XMFLOAT4 GetTint() const;
	ComPtr<ID3D11VertexShader> GetVertexShader() const;
	ComPtr<ID3D11PixelShader> GetPixelShader() const;

	void SetUVScale(XMFLOAT2 newScale);
	void SetUVOffset(XMFLOAT2 newOffset);
	void SetTint(XMFLOAT4 newTint);
	void SetVertexShader(ComPtr<ID3D11VertexShader> newVertexShader);
	void SetPixelShader(ComPtr<ID3D11PixelShader> newPixelShader);
};

