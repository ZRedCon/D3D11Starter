#include "MasterShaderInvludes.hlsli"

cbuffer PixelColorBuffer : register(b0)
{
    Light light1[5];
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

float4 lightCalcPhong(Light l, float3 normal, float3 dirToCamera)
{
    float3 refl = reflect(l.Direction, normal);
    float RdotV = saturate(dot(refl, dirToCamera));
    return AS_4(pow(RdotV, 256) * l.Color * l.Intensity);
}

float4 directionalLightTerms(Light l, VertexToPixel input)
{
    return lightCalcDiffuse(l, input.normal) + lightCalcPhong(l, input.normal, normalize(cameraPosition - input.worldPosition));
}

float4 setOfDirectionalLightTerms(Light l[5], VertexToPixel input)
{
    float4 sum = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < 3; i++)
    {
        sum += directionalLightTerms(l[i], input);
    }

    return sum;
}

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    float2 adjustedUV = input.uv * uvScale + uvOffset;
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, adjustedUV);
    
    float4 ambientTerm = AS_4(ambiantColor) * surfaceColor * color;
    
    return ambientTerm + surfaceColor * setOfDirectionalLightTerms(light1, input);
}
