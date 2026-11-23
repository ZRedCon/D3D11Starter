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
#define MIN_ROUGHNESS                   0.0000001f

// I could not figure out how to do this otherwise
#define TAU                             6.28318530f
#define PI                              3.14159265f
#define HALF_PI                         1.57079632f

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

//deprecated
float4 lightCalcPhong(Light l, float3 normal, float3 dirToCamera, float3 dirToLight)
{
    float3 refl = reflect(-dirToLight, normal);
    float RdotV = saturate(dot(refl, dirToCamera));
    return AS_4(pow(RdotV, 256) * l.Color * l.Intensity);
}


float cookTorranceD(float3 normal, float3 halfAngle, float roughness)
{
    float NdotH = saturate(dot(normal, halfAngle));
    float NDHSquared = NdotH * NdotH;
    float alpha = roughness * roughness;
    float alphaSquared = max(alpha * alpha, MIN_ROUGHNESS);
    
    float denomRoot2 = NDHSquared * (alphaSquared - 1) + 1;
    return alphaSquared / (PI * denomRoot2 * denomRoot2);
}

float cookTorranceG(float3 normal, float3 v, float roughness)
{
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(normal, v));
    return 1 / (NdotV * (1 - k) + k);
}

float3 cookTorranceF(float3 v, float3 halfAngle, float3 f0)
{
    float VdotH = saturate(dot(v, halfAngle));
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}

float3 lightCalcCookTorrence(
        float3 normal, float3 dirToCamera,
        float3 dirToLight, float roughness,
        float3 halfAngle, float3 f0,
        out float3 F)
{
    
    float  D = cookTorranceD(normal, halfAngle, roughness);
    float G = cookTorranceG(normal, dirToCamera, roughness) * cookTorranceG(normal, dirToLight, roughness);
    F = cookTorranceF(dirToCamera, halfAngle, f0);

    float3 specularResult = (D * F * G) / 4;
    
    return specularResult * saturate(dot(normal, dirToLight));
}

float3 diffuseEnergyConserve(float3 diffuse, float3 f, float metal)
{
    return diffuse * (1 - f) * (1 - metal);
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

float3 lightCalc(Light l, VertexToPixel input, float3 cameraPosition, float3 surfaceRGB, float roughness, float metal, float3 f0)
{
    float3 dirToLight = l.Type == LIGHT_TYPE_DIRECTIONAL ? -l.Direction : normalize(l.Position - input.worldPosition);
    float3 dirToCamera = normalize(cameraPosition - input.worldPosition);
    
    float3 halfAngle = normalize(dirToLight + dirToCamera);
    float3 diff = saturate(dot(input.normal, dirToLight));
    
    float3 Fout;
    float3 spec = lightCalcCookTorrence(input.normal, dirToCamera, dirToLight, 
                                        roughness, halfAngle, 
                                        f0, Fout);

    float3 balancedDiff = diffuseEnergyConserve(diff, Fout, metal);
    
    float factor = 1.0;
    if (l.Type == LIGHT_TYPE_SPOT)
    {
        float3 lightToPixel = normalize(input.worldPosition - l.Position);
        float pixelAngle = saturate(dot(lightToPixel, l.Direction));

        float cosOuter = cos(l.SpotOuterAngle);
        float cosInner = cos(l.SpotInnerAngle);
        float falloffRange = cosOuter - cosInner;

        factor *= saturate((cosOuter - pixelAngle) / falloffRange);
    }
    if (l.Type == LIGHT_TYPE_POINT || l.Type == LIGHT_TYPE_SPOT)
    {
        factor *= attenuate(l, input.worldPosition);
    }
    
    return (balancedDiff * surfaceRGB + spec) * l.Intensity * l.Color * factor;
}

inline float4 gammaCorrect(float4 color)
{
    return float4(pow(color.rgb, GAMMA_CORRECTION_EXPONENT), color.a);
}

inline float4 unGammaCorrect(float4 color)
{
    return float4(pow(color.rgb, UNGAMMA_CORRECTION_EXPONENT), color.a);
}

#endif