#include "MasterShaderInvludes.hlsli"

TextureCube SkyTexture : register(t1);
SamplerState BasicSampler : register(s0);

float4 main( SkyVertexToPixel input ) : SV_TARGET
{
    return SkyTexture.Sample(BasicSampler, input.sampleDir);
}