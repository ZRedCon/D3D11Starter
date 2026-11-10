#pragma once
#include <array>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <cstddef>
#include <memory>
#include <iostream>

#include <fstream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <WICTextureLoader.h>

#include "Graphics.h"
#include "Vertex.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "Mesh.h"

class Sky
{
private:
	ComPtr<ID3D11SamplerState> sampler;
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11DepthStencilState> depthStencil;
	ComPtr<ID3D11RasterizerState> rasterizer;

	ComPtr<ID3D11PixelShader> ps;
	ComPtr<ID3D11VertexShader> vs;

	std::shared_ptr<Mesh> mesh; 

public:
	Sky(std::shared_ptr<Mesh> _mesh, ComPtr<ID3D11SamplerState> _sampler, 
		ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader, 
		std::wstring paths[6]);
	ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

	void Draw();
};
