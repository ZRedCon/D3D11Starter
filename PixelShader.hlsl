#include "MasterShaderInvludes.hlsli"

cbuffer PixelColorBuffer : register(b0)
{
    Light light1;
    float4 color;
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPosition;
    float elaspedTime;
    float3 ambiantColor;
}

Texture2D SurfaceTexture  : register(t0);
SamplerState BasicSampler : register(s0);

float4 lightCalcDiffuse(Light l, float3 normal)
{
    float3 dirToLight = -l.Direction;
    return AS_4(saturate(dot(normal, dirToLight)) * l.Color * l.Intensity);
}

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    float2 adjustedUV = input.uv * uvScale + uvOffset;
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, adjustedUV);
    float4 ambientTerm = AS_4(ambiantColor) * surfaceColor * color;
    float4 diffuseTerm = lightCalcDiffuse(light1, input.normal) * surfaceColor;
    return ambientTerm + diffuseTerm;
}
