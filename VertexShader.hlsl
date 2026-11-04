#include "MasterShaderInvludes.hlsli"

cbuffer BufferStruct : register(b0)
{
    matrix world;
    matrix worldInvTranspose;
    matrix view;
    matrix projection;
}

VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;
    matrix wvp = mul(projection, mul(view, world));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz;
    output.uv = input.uv;
    output.normal = mul((float3x3)worldInvTranspose, input.normal);
    
	return output;
}