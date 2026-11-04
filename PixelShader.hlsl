#include "MasterShaderInvludes.hlsli"

cbuffer PixelColorBuffer : register(b0)
{
    float4 color;
    float2 uvScale;
    float2 uvOffset;
    float3 cameraPosition;
    float elaspedTime;
}

Texture2D SurfaceTexture  : register(t0);
SamplerState BasicSampler : register(s0);


float4 main(VertexToPixel input) : SV_TARGET
{
    float2 adjustedUV = input.uv * uvScale + uvOffset;
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, adjustedUV);
    return surfaceColor * color;
}