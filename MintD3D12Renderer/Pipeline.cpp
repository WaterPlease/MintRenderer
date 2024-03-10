#include "pch.h"
#include "Pipeline.h"

#include "Device.h"

using namespace MintChoco;

cPipelineStateObject::cPipelineStateObject() {

}

cPipelineStateObject::~cPipelineStateObject() {
	Destroy();
}

void cPipelineStateObject::Destroy() {
	PipelineStatePtr.Reset();
}

bool cGraphicsPipelineStateObject::Create(cDevice& Device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc) {
	if (IsCreated())
		return false;

	if (Device.GetDevice()->CreateGraphicsPipelineState(&Desc, IID_PPV_ARGS(PipelineStatePtr.ReleaseAndGetAddressOf())) != S_OK) {
		return false;
	}

	return true;
}