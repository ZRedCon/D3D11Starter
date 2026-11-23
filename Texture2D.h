#pragma once
#include <DirectXMath.h>
#include <array>
#include <d3d11_1.h>
#include <memory>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include <d3d11shadertracing.h>
#include <wrl/client.h>
#include <WICTextureLoader.h>
#include <unordered_map>

#include "PathHelpers.h"
#include "Graphics.h"
#include "Vertex.h"

typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRVPtr;

class Texture2D
{
private:
	static std::unordered_map<std::wstring, std::shared_ptr<Texture2D>> textures;
	static inline std::wstring ASSETS_TEXTURE_PATH = L"../../Assets/Textures/";

	SRVPtr srv{};

public:
	Texture2D(std::wstring pathName);
	SRVPtr GetSRV();

	operator SRVPtr() const;

	static SRVPtr GetSRV(std::wstring key);
	static std::shared_ptr<Texture2D> GetTexture(std::wstring key);
};

