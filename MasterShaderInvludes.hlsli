#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__

#define AS_4(f3) float4(f3, 1)

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexToPixel
{
    float3 worldPosition : POSITION;
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

#endif