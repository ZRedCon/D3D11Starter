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

Texture2D SurfaceTexture  : register(t0);
Texture2D NormalMap : register(t1);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    //input.normal = normalize(input.normal);
    float2 adjustedUV = input.uv * uvScale + uvOffset;
    
    float3 unpackedNormals = normalize(NormalMap.Sample(BasicSampler, adjustedUV).rgb * 2.0f - 1.0f);
    float3 normals = normalize(input.normal);
    float3 tangents = normalize(input.tangent - dot(input.tangent, normals) * normals);
    float3 biTangents = cross(tangents, normals);
    float3x3 tanBiNorms = float3x3(tangents, biTangents, normals);
    input.normal = mul(unpackedNormals, tanBiNorms);
    
    float4 surfaceColor = SampleTexture(SurfaceTexture, BasicSampler, adjustedUV);
    float4 ambientTerm = AS_4(ambiantColor) * surfaceColor * color;
    
    float4 sumOfLights = float4(0, 0, 0, 0);
    for (int i = 0; i < 5; i++)
    {
        switch (light1[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                sumOfLights += directionalLightTerms(light1[i], input, cameraPosition);
                break;
            case LIGHT_TYPE_POINT:
                sumOfLights += pointLightTerms(light1[i], input, cameraPosition);
                break;
            case LIGHT_TYPE_SPOT:
                sumOfLights += spotLightTerms(light1[i], input, cameraPosition);
                break;
        }
    }
    
    
    return gammaCorrect(ambientTerm + surfaceColor * sumOfLights);
}
