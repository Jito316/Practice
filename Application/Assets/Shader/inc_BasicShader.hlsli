struct VSOutput
{
    float4 pos : SV_Position;
    float2 uv  : TEXCOORD;
};

cbuffer cbProj : register(b0)
{
    row_major matrix c_mView;
    row_major matrix c_mProj;
}