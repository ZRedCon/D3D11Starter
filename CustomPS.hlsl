#include "MasterShaderInvludes.hlsli"

cbuffer PixelColorBuffer : register(b0)
{
    float4 color;
    float2 uvScale;
    float2 uvOffset;
    float elaspedTime;
}

float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    float factor = abs(sin(2.5 * elaspedTime)) / 2.5 + 0.6;
    factor *= frac(sin(dot(float2(input.screenPosition.x + elaspedTime / 20, input.screenPosition.y + elaspedTime / 20), float2(12.9898, 78.233))));
	
    float4 result = float4(abs(input.normal), 1) * factor;
	
    return result;
}