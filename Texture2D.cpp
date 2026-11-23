#include "Texture2D.h"

Texture2D::Texture2D(std::wstring pathName)
{
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(ASSETS_TEXTURE_PATH + pathName).c_str(), 0, srv.GetAddressOf());

	textures[pathName] = std::shared_ptr<Texture2D>(this);
}

SRVPtr Texture2D::GetSRV() { return srv; }
Texture2D::operator SRVPtr() const { return srv; }

SRVPtr Texture2D::GetSRV(std::wstring key)
{
	return textures.contains(key) ? textures[key]->GetSRV() : Texture2D(key);
}

std::shared_ptr<Texture2D> Texture2D::GetTexture(std::wstring key)
{
	if (!textures.contains(key)) auto blank = Texture2D(key);
	return textures[key];
}
