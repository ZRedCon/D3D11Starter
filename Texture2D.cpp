#include "Texture2D.h"

#define GetAssetPath(subpath) FixPath(ASSETS_TEXTURE_PATH + subpath).c_str()

Texture2D::Texture2D(std::wstring pathName)
{
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		GetAssetPath(L"Textures/Textures with Normal Maps/cobblestone.png"), 0, srv.GetAddressOf());
}

SRVPtr Texture2D::GetSRV()
{
	return srv;
}
