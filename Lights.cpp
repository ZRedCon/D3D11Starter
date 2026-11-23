#include "Lights.h"
#include "BufferStructs.h"

void LightObject::PlaceLightsInPS(PixelColorBuffer& psData)
{
	int i = 0;
	for (auto& lightObject : GetInstances())
	{
		if (i == NUM_LIGHTS) { printf("Error: Too many lights\n"); break; }
		psData.light[i++] = *lightObject;   // uses operator Light()
	}
}

LightObject::LightObject(std::string _name, int type)
{
	name = _name;
	light.Type = type;
}

LightObject::LightObject(std::string _name, const LightObject& other)
{
	name = _name;
	light = other.light;
}

LightObject::LightObject(std::string _name, const Light& other)
{
	name = _name;
	light = other;
}

LightObject& LightObject::operator=(const LightObject& right)
{
	if (this == &right) return *this;
	light = right.light;
	return *this;
}

LightObject& LightObject::operator=(const Light& right)
{
	light = right;
	return *this;
}

LightObject& LightObject::SetRange(float range)
{
	light.Range = range;
	return *this;
}
LightObject& LightObject::SetIntensity(float intensity)
{
	light.Intensity = intensity;
	return *this;
}

LightObject& LightObject::SetPadding(float x, float y) { return SetPadding(DirectX::XMFLOAT2(x,y )); }
LightObject& LightObject::SetPadding(DirectX::XMFLOAT2 padding)
{
	light.Padding = padding;
	return *this;
}

LightObject& LightObject::SetDirection(float x, float y, float z)
{
	DirectX::XMVECTOR vector = DirectX::XMVectorSet(x,y,z,0.0f);
	DirectX::XMStoreFloat3(&light.Direction, DirectX::XMVector3Normalize(vector));

	return *this;
}
LightObject& LightObject::SetDirection(DirectX::XMFLOAT3 direction)
{
	DirectX::XMStoreFloat3(&light.Direction, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)));
	return *this;
}

LightObject& LightObject::SetPosition(float x, float y, float z) { return SetPosition(DirectX::XMFLOAT3(x,y,z)); }
LightObject& LightObject::SetPosition(DirectX::XMFLOAT3 position)
{
	light.Position = position;
	return *this;
}

LightObject& LightObject::SetColor(float r, float g, float b) { return SetColor(DirectX::XMFLOAT3(r, g, b)); }
LightObject& LightObject::SetColor(DirectX::XMFLOAT3 color)
{
	light.Color = color;
	return *this;
}

LightObject& LightObject::SetSpotAngles(float inner, float outer)
{
	SetSpotInnerAngle(inner);
	return SetSpotOuterAngle(outer);
}
LightObject& LightObject::SetSpotAnglesDegrees(float inner, float outer)
{
	return SetSpotAngles(DirectX::XMConvertToRadians(inner), DirectX::XMConvertToRadians(outer));
}
LightObject& LightObject::SetSpotInnerAngle(float innerAngle)
{
	light.SpotInnerAngle = innerAngle;
	return *this;
}
LightObject& LightObject::SetSpotOuterAngle(float outerAngle)
{
	light.SpotOuterAngle = outerAngle;
	return *this;
}

LightObject::operator Light() const
{
	return light;
}

static std::string GetLightType(Light l)
{
	switch (l.Type)
	{
	case LIGHT_TYPE_DIRECTIONAL: return "DIRECTIONAL";
	case LIGHT_TYPE_POINT: return "POINT";
	case LIGHT_TYPE_SPOT: return "SPOT";
	default: return "Invalid Type, something has gone horribly, horribly wrong!";
	}
}

void LightObject::Inspector()
{
	if (ImGui::CollapsingHeader(name.c_str())) {
		ImGui::Indent();
		ImGui::LabelText("", ("Type: " + GetLightType(light)).c_str());
		ImGui::ColorEdit3((name + "'s Color").c_str(), &light.Color.x);
		ImGui::DragFloat((name + "'s Intensity").c_str(), &light.Intensity, 0.05f, 0.0f, 200.0f);
		ImGui::Unindent();
	}
}

void LightObject::InspectStatics()
{
	
}
