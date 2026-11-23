#include "Texture2D.h"

Texture2D::Texture2D(std::string _name, std::wstring pathName)
{
	name = _name;
	std::wstring fullPath = FixPath(ASSETS_TEXTURE_PATH + pathName);
	auto hr = DirectX::CreateWICTextureFromFile(
		Graphics::Device.Get(), Graphics::Context.Get(), 
		fullPath.c_str(), nullptr, 
		srv.GetAddressOf());

	if (FAILED(hr)) printf("Asset Err, Texture2D couldn't find asset at %ls\n", fullPath.c_str());
}

SRVPtr Texture2D::GetSRV() { return srv; }
Texture2D::operator SRVPtr() const { return srv; }
SRVPtr Texture2D::GetSRV(std::string key) { return Get(key)->GetSRV(); }

void Texture2D::Inspector()
{
	ImGui::Indent();
	ImGui::Image((void*)(srv.Get()), ImVec2(40.0f, 40.0f));
	ImGui::Unindent();
}

void Texture2D::InspectStatics()
{

}
