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

XMFLOAT4 Material::GetTint() const { return tint; }
ComPtr<ID3D11VertexShader> Material::GetVertexShader() const { return vertexShader; }
ComPtr<ID3D11PixelShader> Material::GetPixelShader() const { return pixelShader; }

void Material::SetTint(XMFLOAT4 newTint)
{
	tint = newTint;
}

void Material::SetVertexShader(ComPtr<ID3D11VertexShader> newVertexShader)
{
	vertexShader = newVertexShader;
}

void Material::SetPixelShader(ComPtr<ID3D11PixelShader> newPixelShader)
{
	pixelShader = newPixelShader;
}