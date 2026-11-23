#include "MasterShaderIncludes.hlsli"

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

Texture2D SurfaceTexture    : register(t0);
Texture2D NormalMap         : register(t1);
Texture2D RoughnessMap      : register(t2);
Texture2D MetalnessMap      : register(t3);
SamplerState BasicSampler   : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float2 adjustedUV = input.uv * uvScale + uvOffset;
    
    float3 unpackedNormals = normalize(NormalMap.Sample(BasicSampler, adjustedUV).rgb * 2.0f - 1.0f);
    float3 normals = normalize(input.normal);
    float3 tangents = normalize(input.tangent - dot(input.tangent, normals) * normals);
    float3 biTangents = cross(tangents, normals);
    float3x3 tanBiNorms = float3x3(tangents, biTangents, normals);
    input.normal = mul(unpackedNormals, tanBiNorms);
    
    float roughness = 0.5; //RoughnessMap.Sample(BasicSampler, adjustedUV).r;
    float metalness = 0.5; //MetalnessMap.Sample(BasicSampler, adjustedUV).r;
    float4 surfaceColor = SampleTexture(SurfaceTexture, BasicSampler, adjustedUV) * AS_4(color.rgb);
    float3 f0 = lerp(0.04, surfaceColor, metalness) * (1 - metalness);
    
    float3 ambientTerm = ambiantColor * surfaceColor.rgb;
    
    float3 sumOfLights = float3(0, 0, 0);
    for (int i = 0; i < 5; i++)
    {
        sumOfLights += lightCalc(light1[i], input, cameraPosition, surfaceColor.rgb, roughness, metalness, f0);

    }
    
    return gammaCorrect(AS_4(ambientTerm + sumOfLights));
}
