#pragma once
#include <DirectXMath.h>
#include "BufferStructs.h"

#include "Inspectable.h"

#define NUM_LIGHTS 5

#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

struct Light
{
	int					Type;
	DirectX::XMFLOAT3	Direction;
	float				Range;
	DirectX::XMFLOAT3	Position;
	float				Intensity;
	DirectX::XMFLOAT3	Color;
	float				SpotInnerAngle;
	float				SpotOuterAngle;
	DirectX::XMFLOAT2	Padding;
};

class LightObject : public Registered<LightObject>
{
private:
	Light light{};

public:
	static void PlaceLightsInPS(PixelColorBuffer& psData);

	LightObject(int type);

	LightObject(const LightObject& other);
	LightObject(const Light& other);

	LightObject& operator=(const LightObject& right);
	LightObject& operator=(const Light& right);

	LightObject& SetRange(float range);
	LightObject& SetIntensity(float intensity);

	LightObject& SetPadding(float x, float y);
	LightObject& SetPadding(DirectX::XMFLOAT2 padding);

	LightObject& SetDirection(float x, float y, float z);
	LightObject& SetDirection(DirectX::XMFLOAT3 direction);

	LightObject& SetPosition(float x, float y, float z);
	LightObject& SetPosition(DirectX::XMFLOAT3 position);

	LightObject& SetColor(float r, float g, float b);
	LightObject& SetColor(DirectX::XMFLOAT3 color);

	LightObject& SetSpotAngles(float inner, float outer);
	LightObject& SetSpotAnglesDegrees(float inner, float outer);
 	LightObject& SetSpotInnerAngle(float innerAngle);
	LightObject& SetSpotOuterAngle(float outerAngle);

	operator Light() const;

	void Inspector() override;
	void InspectStatics() override;
};

