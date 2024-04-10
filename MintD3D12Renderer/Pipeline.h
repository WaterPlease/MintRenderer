#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Utilities.h"

namespace MintChoco {
	class cDevice;
	using Microsoft::WRL::ComPtr;

	class cPipelineStateObject {
		DELETE_CLASS_COPY(cPipelineStateObject);
	public:
		cPipelineStateObject();
		virtual ~cPipelineStateObject();

		void Destroy();

		bool IsCreated() const { return PipelineStatePtr.Get(); }
		ID3D12PipelineState* GetPipelineState() const { return PipelineStatePtr.Get(); }
	protected:
		ComPtr<ID3D12PipelineState> PipelineStatePtr;
	};

	class cPipelineLibrary {
		
	};

	class cGraphicsPipelineStateObject : public cPipelineStateObject {
		DELETE_CLASS_COPY(cGraphicsPipelineStateObject);
	public:
		bool Create(cDevice& Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc);

	protected:
	};
}

