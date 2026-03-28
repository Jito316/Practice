#include "inc_BasicShader.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput Out;
    Out.pos = mul(pos, c_mView);
    Out.pos = mul(Out.pos, c_mProj);
    Out.uv = uv;
	
    return Out;
}