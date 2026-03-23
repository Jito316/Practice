#include "inc_BasicShader.hlsli"

float4 main(VSOutput In) : SV_TARGET
{
    return float4(In.uv.r, In.uv.g, 0.0f, 1.0f);
}