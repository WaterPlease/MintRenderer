#include "TriangleRS.hlsli"

struct VSIn
{
    float3 Pos : Position;
    float2 TexCoord : TexCoord;
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
    Out.Pos = float4(In.Pos, 1.0);
    Out.TexCoord = In.TexCoord;
    return Out;
}