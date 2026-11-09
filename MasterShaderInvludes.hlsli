#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__

#define AS_4(f3) float4(f3, 1)

#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float3 worldPosition : POSITION;
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct SkyVertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

struct Light
{
    int Type;
    float3 Direction;
    float Range;
    float3 Position;
    float Intensity;
    float3 Color;
    float SpotInnerAngle;
    float SpotOuterAngle;
    float2 Padding;
};

#endif