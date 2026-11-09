#include "MasterShaderInvludes.hlsli"

cbuffer SkyBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
}

SkyVertexToPixel main(VertexShaderInput input)
{
    SkyVertexToPixel output;
    
    matrix viewNoTranslation = viewMatrix;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    output.position = mul(projectionMatrix, mul(viewMatrix, AS_4(input.localPosition)));
    output.position = output.position.xyww;
    
    output.sampleDir = input.localPosition;
    
	return output;
}