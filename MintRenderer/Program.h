#pragma once
#include <cassert>

#include "Utilities.h"
#include "Pipeline.h"
#include "Data.h"
#include <type_traits>
#include <unordered_map>


namespace MintChoco {
	struct sVariableGraphicsPipelineStateDesc {
		D3D12_BLEND_DESC BlendState;
		UINT SampleMask;
		D3D12_RASTERIZER_DESC RasterizerState;
		D3D12_DEPTH_STENCIL_DESC DepthStencilState;
		D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
		UINT NumRenderTargets;
		DXGI_FORMAT RTVFormats[_countof(D3D12_GRAPHICS_PIPELINE_STATE_DESC::RTVFormats)];
		DXGI_FORMAT DSVFormat;
		DXGI_SAMPLE_DESC SampleDesc;
		UINT NodeMask;
		D3D12_PIPELINE_STATE_FLAGS Flags;

		sVariableGraphicsPipelineStateDesc& operator=(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& PSODesc);
		bool operator==(const sVariableGraphicsPipelineStateDesc&) const;

		void ExtractFrom(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& PSODesc);
		void ApplyTo(D3D12_GRAPHICS_PIPELINE_STATE_DESC& PSODesc) const;
	};
}

template<>
struct std::hash<D3D12_RENDER_TARGET_BLEND_DESC> {
	std::size_t operator()(const D3D12_RENDER_TARGET_BLEND_DESC& Desc) const noexcept {
		size_t Hash = HashCombine(0, Desc.BlendEnable, Desc.LogicOpEnable, Desc.RenderTargetWriteMask);

		if (Desc.BlendEnable == TRUE) {
			Hash = HashCombine(Hash,
				Desc.BlendOp,
				Desc.BlendOpAlpha,
				Desc.SrcBlend,
				Desc.SrcBlendAlpha,
				Desc.DestBlend,
				Desc.DestBlendAlpha);
		}

		if (Desc.LogicOpEnable == TRUE) {
			Hash = HashCombine(Hash, Desc.LogicOp);
		}

		return Hash;
	}
};

template<>
struct std::hash<D3D12_RASTERIZER_DESC> {
	std::size_t operator()(const D3D12_RASTERIZER_DESC& Desc) const noexcept {
		/*
		  D3D12_FILL_MODE                       FillMode;
		  D3D12_CULL_MODE                       CullMode;
		  BOOL                                  FrontCounterClockwise;
		  INT                                   DepthBias;
		  FLOAT                                 DepthBiasClamp;
		  FLOAT                                 SlopeScaledDepthBias;
		  BOOL                                  DepthClipEnable;
		  BOOL                                  MultisampleEnable;
		  BOOL                                  AntialiasedLineEnable;
		  UINT                                  ForcedSampleCount;
		  D3D12_CONSERVATIVE_RASTERIZATION_MODE ConservativeRaster;
		*/

		return HashCombine(0,
							Desc.FillMode,
							Desc.CullMode,
							Desc.FrontCounterClockwise,
							Desc.DepthBias,
							Desc.DepthBiasClamp,
							Desc.SlopeScaledDepthBias,
							Desc.DepthClipEnable,
							Desc.MultisampleEnable,
							Desc.AntialiasedLineEnable,
							Desc.ForcedSampleCount,
							Desc.ConservativeRaster);
	}
};

template<>
struct std::hash<D3D12_DEPTH_STENCILOP_DESC> {
	std::size_t operator()(const D3D12_DEPTH_STENCILOP_DESC& Desc) const noexcept {
		/*
		    D3D12_STENCIL_OP StencilFailOp;
		    D3D12_STENCIL_OP StencilDepthFailOp;
		    D3D12_STENCIL_OP StencilPassOp;
		    D3D12_COMPARISON_FUNC StencilFunc;
		*/

		return HashCombine(0, Desc.StencilFailOp, Desc.StencilDepthFailOp, Desc.StencilPassOp, Desc.StencilFunc);
	}
};

template<>
struct std::hash<D3D12_DEPTH_STENCIL_DESC> {
	std::size_t operator()(const D3D12_DEPTH_STENCIL_DESC& Desc) const noexcept {
		/*
		    BOOL DepthEnable;
		    D3D12_DEPTH_WRITE_MASK DepthWriteMask;
		    D3D12_COMPARISON_FUNC DepthFunc;
		    BOOL StencilEnable;
		    UINT8 StencilReadMask;
		    UINT8 StencilWriteMask;
		    D3D12_DEPTH_STENCILOP_DESC FrontFace;
		    D3D12_DEPTH_STENCILOP_DESC BackFace;
		*/
		return HashCombine(0,
			Desc.DepthEnable,
			Desc.DepthWriteMask,
			Desc.DepthFunc,
			Desc.StencilEnable,
			Desc.StencilReadMask,
			Desc.StencilWriteMask,
			Desc.FrontFace,
			Desc.BackFace);
	}
};

template<>
struct std::hash<DXGI_SAMPLE_DESC> {
	std::size_t operator()(const DXGI_SAMPLE_DESC& Desc) const noexcept {
		/*
		    UINT Count;
		    UINT Quality;
		*/
		return HashCombine(0,
			Desc.Count,
			Desc.Quality);
	}
};

template<>
struct std::hash<MintChoco::sVariableGraphicsPipelineStateDesc> {
	std::size_t operator()(const MintChoco::sVariableGraphicsPipelineStateDesc& Desc) const noexcept {
		size_t Hash = 0;
		/*
		    D3D12_BLEND_DESC BlendState;
		    UINT SampleMask;
		    D3D12_RASTERIZER_DESC RasterizerState;
		    D3D12_DEPTH_STENCIL_DESC DepthStencilState;
		    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
		    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
		    UINT NumRenderTargets;
		    DXGI_FORMAT RTVFormats[ 8 ];
		    DXGI_FORMAT DSVFormat;
		    DXGI_SAMPLE_DESC SampleDesc;
		    UINT NodeMask;
		    D3D12_PIPELINE_STATE_FLAGS Flags; 
		*/

		// BlendState
		Hash = HashCombine(Hash,
						Desc.BlendState.AlphaToCoverageEnable,
							Desc.BlendState.IndependentBlendEnable);
		size_t NumRTBlendDesc = Desc.NumRenderTargets;
		assert(Desc.BlendState.IndependentBlendEnable == TRUE || Desc.BlendState.IndependentBlendEnable == FALSE);
		if (Desc.BlendState.IndependentBlendEnable == FALSE) {
			NumRTBlendDesc = 1;
		}
		for (size_t iRT = 0; iRT < NumRTBlendDesc; ++iRT) {
			Hash = HashCombine(Hash, Desc.BlendState.RenderTarget[iRT]);
		}
		Hash = HashCombine(Hash, Desc.SampleMask, Desc.NodeMask, Desc.DSVFormat, Desc.NumRenderTargets);
		for (size_t iRT = 0; iRT < Desc.NumRenderTargets; ++iRT) {
			Hash = HashCombine(Hash, Desc.RTVFormats[iRT]);
		}

		Hash = HashCombine(Hash, Desc.RasterizerState, Desc.DepthStencilState, Desc.IBStripCutValue, Desc.PrimitiveTopologyType, Desc.SampleDesc, Desc.Flags);

		return Hash;
	}
};

namespace MintChoco {
	class cGraphicsProgram {
		DELETE_CLASS_COPY(cGraphicsProgram)
	public:
		cGraphicsProgram();
		~cGraphicsProgram();

		// TemplateDesc must be set before Build method called.
		bool Build();
		void Unbuild();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetTemplate() { return TemplateDesc; }
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetTemplate() const { return TemplateDesc; }

		const cGraphicsPipelineStateObject* GetPSO(const sVariableGraphicsPipelineStateDesc& Desc) const;
	protected:
		std::unordered_map<sVariableGraphicsPipelineStateDesc, cGraphicsPipelineStateObject*> PSOMap;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC TemplateDesc;
	};
}
	


