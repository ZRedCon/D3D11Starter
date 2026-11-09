#include "Material.h"

Material::Material(XMFLOAT4 startTint, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader)
{
	tint = startTint;
	vertexShader = startVertexShader;
	pixelShader = startPixelShader;
}

Material::Material(XMFLOAT3 startTint, ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader) :
	Material(XMFLOAT4(startTint.x, startTint.y, startTint.z, 1.0f), startVertexShader, startPixelShader) {}

Material::Material(ComPtr<ID3D11VertexShader> startVertexShader, ComPtr<ID3D11PixelShader> startPixelShader) :
	Material(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), startVertexShader, startPixelShader) {}

Material& Material::AddTextureSRVAndSampler(unsigned int slot, ComPtr<ID3D11ShaderResourceView> srv, ComPtr<ID3D11SamplerState> sampler)
{
	AddTextureSRV(slot, srv);
	AddSampler(slot, sampler);
	return *this;
}

Material& Material::AddTextureSRV(unsigned int slot, ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs[ slot ] = srv;
	return *this;
}

Material& Material::AddSampler(unsigned int slot, ComPtr<ID3D11SamplerState> sampler)
{
	samplers[ slot ] = sampler;
	return *this;
}

void Material::BindTexturesAndSamplers()
{
	for (int i = 0; i < textureSRVs.size(); i++)
		Graphics::Context->PSSetShaderResources(i, 1, textureSRVs[ i ].GetAddressOf());

	for (int i = 0; i < samplers.size(); i++)
		Graphics::Context->PSSetSamplers(i, 1, samplers[ i ].GetAddressOf());
}

XMFLOAT2 Material::GetUVScale() const { return uvScale; }
XMFLOAT2 Material::GetUVOffset() const { return uvOffset; }
XMFLOAT4 Material::GetTint() const { return tint; }
ComPtr<ID3D11VertexShader> Material::GetVertexShader() const { return vertexShader; }
ComPtr<ID3D11PixelShader> Material::GetPixelShader() const { return pixelShader; }

Material& Material::SetUVScale(XMFLOAT2 newScale)
{
	uvScale = newScale;
	return *this;
}

Material& Material::SetUVOffset(XMFLOAT2 newOffset)
{
	uvOffset = newOffset;
	return *this;
}

Material& Material::SetTint(XMFLOAT4 newTint)
{
	tint = newTint;
	return *this;
}

Material& Material::SetVertexShader(ComPtr<ID3D11VertexShader> newVertexShader)
{
	vertexShader = newVertexShader;
	return *this;
}

Material& Material::SetPixelShader(ComPtr<ID3D11PixelShader> newPixelShader)
{
	pixelShader = newPixelShader;
	return *this;
}

void Material::Inspector(std::string name)
{
	if (ImGui::CollapsingHeader((name + " Material").c_str()))
	{
		ImGui::Indent();

		ImGui::ColorEdit4((name + " Tint").c_str(), &tint.x);
		ImGui::DragFloat2((name + " UV Sacle").c_str(), &uvScale.x, 0.05f);
		ImGui::DragFloat2((name + " UV Offset").c_str(), &uvOffset.x, 0.05f);

		if (ImGui::CollapsingHeader((name + " Textures").c_str()))
		{
			for (int i = 0; i < textureSRVs.size(); i++)
			{
				if (textureSRVs[i] == nullptr) break;
				ImGui::Image((void*)(textureSRVs[i].Get()), ImVec2(40.0f, 40.0f));
			}
		}

		ImGui::Unindent();
	}
}
