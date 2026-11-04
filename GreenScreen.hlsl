#include "MasterShaderInvludes.hlsli"

Texture2D SurfaceTexture : register(t0);
Texture2D OverlapTexture : register(t1);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);
    float4 overlapColor = OverlapTexture.Sample(BasicSampler, input.uv);
    overlapColor = float4(overlapColor.r, overlapColor.r, overlapColor.r, 1 - overlapColor.g);
    return surfaceColor * overlapColor;
}