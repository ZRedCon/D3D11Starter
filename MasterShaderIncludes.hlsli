#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__

// Macro functions / operations
#define AS_4(f3) float4(f3, 1)
#define SampleTexture(texture, sampler, uvmap) unGammaCorrect(texture.Sample(sampler, uvmap))

// Pseudo-enum for light types
#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

// Macro values
#define UNGAMMA_CORRECTION_EXPONENT     2.2f
#define GAMMA_CORRECTION_EXPONENT       (1.0f / UNGAMMA_CORRECTION_EXPONENT)

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float3 worldPosition : POSITION;
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct SkyVertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

struct Light
{
    int Type;
    float3 Direction;
    float Range;
    float3 Position;
    float Intensity;
    float3 Color;
    float SpotInnerAngle;
    float SpotOuterAngle;
    float2 Padding;
};

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

float4 directionalLightTerms(Light l, VertexToPixel input, float3 cameraPosition)
{
    float3 dirToLight = -l.Direction;
    float3 dirToCamera = normalize(cameraPosition - input.worldPosition);
    float4 diffuse = lightCalcDiffuse(l, input.normal, dirToLight);
    return diffuse + lightCalcPhong(l, input.normal, dirToCamera, dirToLight) * any(diffuse.rgb);
}

float4 pointLightTerms(Light l, VertexToPixel input, float3 cameraPosition)
{
    float3 dirToLight = normalize(l.Position - input.worldPosition);
    float3 dirToCamera = normalize(cameraPosition - input.worldPosition);
    float4 diffuse = lightCalcDiffuse(l, input.normal, dirToLight);
    return (diffuse + lightCalcPhong(l, input.normal, dirToCamera, dirToLight) * any(diffuse.rgb))
           * attenuate(l, input.worldPosition);
}

float4 spotLightTerms(Light l, VertexToPixel input, float3 cameraPosition)
{
    float3 lightToPixel = normalize(input.worldPosition - l.Position);
    float pixelAngle = saturate(dot(lightToPixel, l.Direction));

    float cosOuter = cos(l.SpotOuterAngle);
    float cosInner = cos(l.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;

    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);

    return pointLightTerms(l, input, cameraPosition) * spotTerm;
}

inline float4 gammaCorrect(float4 color)
{
    return pow(color, GAMMA_CORRECTION_EXPONENT);
}

inline float4 unGammaCorrect(float4 color)
{
    return pow(color, UNGAMMA_CORRECTION_EXPONENT);
}

#endif