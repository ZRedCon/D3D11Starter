#include "MasterShaderInvludes.hlsli"

cbuffer BufferStruct : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}


VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;
    matrix wvp = mul(projection, mul(view, world));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.uv = input.uv;
    output.normal = input.normal;
    
	return output;
}