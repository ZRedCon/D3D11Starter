cbuffer BufferStruct : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

struct VertexShaderInput
{ 
	float3 localPosition : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;
    matrix wvp = mul(projection, mul(view, world));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.uv = input.uv;
    output.normal = input.normal;
    
	return output;
}