#include "Texture2D.h"

std::unordered_map<std::wstring, std::shared_ptr<Texture2D>> Texture2D::textures;

Texture2D::Texture2D(std::wstring pathName)
{
	std::wstring fullPath = FixPath(ASSETS_TEXTURE_PATH + pathName);
	auto hr = DirectX::CreateWICTextureFromFile(
		Graphics::Device.Get(), 
		Graphics::Context.Get(), 
		fullPath.c_str(), 
		nullptr, 
		srv.GetAddressOf());

	if (FAILED(hr)) printf("Asset Err, Texture2D couldn't find asset at %ls\n", fullPath.c_str());
}

SRVPtr Texture2D::GetSRV() { return srv; }
Texture2D::operator SRVPtr() const { return srv; }
SRVPtr Texture2D::GetSRV(std::wstring key) { return GetTexture(key)->GetSRV(); }

std::shared_ptr<Texture2D> Texture2D::GetTexture(std::wstring key)
{
	auto it = textures.find(key);
	if (it == textures.end())
		it = textures.emplace(key, std::make_shared<Texture2D>(key)).first;
	return it->second;
}