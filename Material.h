#pragma once
#include <DirectXMath.h>
#include <array>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <cstddef>
#include <memory>
#include <iostream>
#include <unordered_map>

#include "Graphics.h"
#include "Vertex.h"
#include "Texture2D.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class Material
{
private:
	static std::unordered_map<std::string, std::shared_ptr<Material>> materials;

	XMFLOAT4 tint;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;

	std::array<SRVPtr, 128> textureSRVs{};
	std::array<ComPtr<ID3D11SamplerState>, 16> samplers{};

	XMFLOAT2 uvScale = XMFLOAT2(1, 1);
	XMFLOAT2 uvOffset = XMFLOAT2(0, 0);
public:
	Material(std::string name, XMFLOAT4 startTint, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);
	Material(std::string name, XMFLOAT3 startTint, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);
	Material(std::string name, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader);
	Material(std::string name);
	
	Material& StorePBRTextures(SRVPtr albedo, SRVPtr normal, SRVPtr roughness, SRVPtr metallic);

	Material& AddTextureSRVAndSampler(unsigned int slot, SRVPtr srv, ComPtr<ID3D11SamplerState> sampler);
	Material& AddTextureSRV(unsigned int slot, SRVPtr srv);
	Material& AddSampler(unsigned int slot, ComPtr<ID3D11SamplerState> sampler);

	void BindTexturesAndSamplers();

	XMFLOAT2 GetUVScale() const;
	XMFLOAT2 GetUVOffset() const;
	XMFLOAT4 GetTint() const;
	ComPtr<ID3D11VertexShader> GetVertexShader() const;
	ComPtr<ID3D11PixelShader> GetPixelShader() const;

	Material& SetUVScale(XMFLOAT2 newScale);
	Material& SetUVOffset(XMFLOAT2 newOffset);
	Material& SetTint(XMFLOAT4 newTint);
	Material& SetVertexShader(ComPtr<ID3D11VertexShader> newVertexShader);
	Material& SetPixelShader(ComPtr<ID3D11PixelShader> newPixelShader);
	Material& SetShaders(ComPtr<ID3D11PixelShader> newPixelShader, ComPtr<ID3D11VertexShader> newVertexShader);

	void Inspector(std::string name);

	static std::shared_ptr<Material> MaterialAt(std::string);
};

