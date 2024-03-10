#include "TriangleRS.hlsli"

cbuffer sTransform : register(b1)
{
    float4x4 Model;
    float4x4 View;
    float4x4 InverseView;
    float4x4 Projection;
};

struct VSIn
{
    float3 Position  : Position0;
    float3 Normal    : Normal0;
    float3 Tangent   : Tangent0;
    float2 TexCoord0 : TexCoord0;
    float2 TexCoord1 : TexCoord1;
    float4 Color     : Color0;
    uint4  Joint     : Joint0;
    float4 Weight    : Weight0;
};

struct VSOut
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TexCoord;
};

[RootSignature(ROOTSIG)]
VSOut main(in VSIn In)
{
    VSOut Out;
    Out.Pos = mul(Model, float4(In.Position, 1.0));
    Out.Pos = mul(View, Out.Pos);
    Out.Pos = mul(Projection, Out.Pos);
    Out.TexCoord = In.TexCoord0;
    return Out;
}