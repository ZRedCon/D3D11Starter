Texture2D SurfaceTexture : register(t0);
Texture2D OverlapTexture : register(t1);
SamplerState BasicSampler : register(s0);

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);
    float4 overlapColor = OverlapTexture.Sample(BasicSampler, input.uv);
    overlapColor = float4(overlapColor.r, overlapColor.r, overlapColor.r, 1 - overlapColor.g);
    return surfaceColor * overlapColor;
}