#include "TriangleRS.hlsli"

float3 color : register(b0);
cbuffer sTransform : register(b1)
{
    float4x4 Model;
    float4x4 View;
    float4x4 InverseView;
    float4x4 Projection;
};
Texture2D<float4> textures[] : register(t0);
sampler textureSampler : register(s0);

struct PSIn
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TexCoord;
};

[RootSignature(ROOTSIG)]
float4 main(PSIn In) : SV_TARGET
{
    float4 Texel = textures[0].Sample(textureSampler, In.TexCoord);
    return float4(Texel.rgb, 1.0f);
}