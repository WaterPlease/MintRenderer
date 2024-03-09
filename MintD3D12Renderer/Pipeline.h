#pragma once

#include <d3d12.h>
#include <wrl.h>

namespace MintChoco {
	class cDevice;
	using Microsoft::WRL::ComPtr;

	class cPipelineStateObject {
	public:
		cPipelineStateObject();
		~cPipelineStateObject();

		void Destroy();

		bool IsCreated() const { return PipelineStatePtr.Get(); }
		ID3D12PipelineState* GetPipelineState() const { return PipelineStatePtr.Get(); }
	protected:
		ComPtr<ID3D12PipelineState> PipelineStatePtr;
	};

	class cPipelineLibrary {
		
	};

	class cGraphicsPipelineStateObject : public cPipelineStateObject {
	public:
		bool Create(cDevice& Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc, cPipelineLibrary* pPipelineLibrary = nullptr);

	protected:
	};
}

