/*
 *  Root Signature Layout (CPU)
 *  - 0     : float3 "Color"
 *  - 1     : Descriptor Table (Transform)
 *      - Transform
 *      - Texture
 *
 *  Root Signature Layout (GPU)
 *  - b0    : float3 "Color"
 *  - b1    : sTransform Transform
 *  - t0... : Texture2D<float4> Textures
 *  - s0    : Sampler for "Textures"
 *
 *  struct sTransform {
 *      float4x4 Model;
 *      float4x4 View;
 *      float4x4 InverseView;
 *      float4x4 Projection;
 *  }
 */


#define ROOTSIG \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),"\
	"RootConstants(num32BitConstants=3, b0),"\
    "DescriptorTable("\
		"CBV(b1, numDescriptors = 1),"\
		"SRV(t0, numDescriptors = 1)"\
    "),"\
    "StaticSampler(s0, filter=FILTER_MIN_MAG_MIP_LINEAR)"