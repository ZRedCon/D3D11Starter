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
Texture2D NormalMap : register(t1);
SamplerState BasicSampler : register(s0);

float4 lightCalcDiffuse(Light l, float3 normal, float3 dirToLight)
{
    return AS_4(saturate(dot(normal, dirToLight)) * l.Color * l.Intensity);
}

float4 lightCalcPhong(Light l, float3 normal, float3 dirToCamera, float3 dirToLight)
{
    float3 refl = reflect(-dirToLight, normal);
    float RdotV = saturate(dot(refl, dirToCamera));
    return AS_4(pow(RdotV, 256) * l.Color * l.Intensity);
}

float attenuate(Light l, float3 worldPos)
{
    float dist = distance(l.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (l.Range * l.Range)));
    return att * att;
}

float4 directionalLightTerms(Light l, VertexToPixel input)
{
    float3 dirToLight = -l.Direction;
    float3 dirToCamera = normalize(cameraPosition - input.worldPosition);
    float4 diffuse = lightCalcDiffuse(l, input.normal, dirToLight);
    return diffuse + lightCalcPhong(l, input.normal, dirToCamera, dirToLight) * any(diffuse.rgb);
}

float4 pointLightTerms(Light l, VertexToPixel input)
{
    float3 dirToLight = normalize(l.Position - input.worldPosition);
    float3 dirToCamera = normalize(cameraPosition - input.worldPosition);
    float4 diffuse = lightCalcDiffuse(l, input.normal, dirToLight);
    return (diffuse + lightCalcPhong(l, input.normal, dirToCamera, dirToLight) * any(diffuse.rgb))
           * attenuate(l, input.worldPosition);
}

float4 spotLightTerms(Light l, VertexToPixel input)
{
    float3 lightToPixel = normalize(input.worldPosition - l.Position);
    float pixelAngle = saturate(dot(lightToPixel, l.Direction));

    float cosOuter = cos(l.SpotOuterAngle);
    float cosInner = cos(l.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;

    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);

    return pointLightTerms(l, input) * spotTerm;
}

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
    
    float4 surfaceColor = SurfaceTexture.Sample(BasicSampler, adjustedUV);
    float4 ambientTerm = AS_4(ambiantColor) * surfaceColor * color;
    
    float4 sumOfLights = float4(0, 0, 0, 0);
    for (int i = 0; i < 5; i++)
    {
        switch (light1[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                sumOfLights += directionalLightTerms(light1[i], input);
                break;
            case LIGHT_TYPE_POINT:
                sumOfLights += pointLightTerms(light1[i], input);
                break;
            case LIGHT_TYPE_SPOT:
                sumOfLights += spotLightTerms(light1[i], input);
                break;
        }
    }
    
    return ambientTerm + surfaceColor * sumOfLights;
}
