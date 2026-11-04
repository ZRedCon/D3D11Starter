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
    return float4(input.normal, 1);
}