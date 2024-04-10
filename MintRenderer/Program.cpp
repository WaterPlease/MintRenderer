#include "pch.h"
#include "Program.h"

using namespace MintChoco;

sVariableGraphicsPipelineStateDesc& sVariableGraphicsPipelineStateDesc::operator=(
	const D3D12_GRAPHICS_PIPELINE_STATE_DESC& PSODesc) {
	BlendState = PSODesc.BlendState;
	SampleMask = PSODesc.SampleMask;
	RasterizerState = PSODesc.RasterizerState;
	DepthStencilState = PSODesc.DepthStencilState;
	IBStripCutValue = PSODesc.IBStripCutValue;
	PrimitiveTopologyType = PSODesc.PrimitiveTopologyType;
	NumRenderTargets = PSODesc.NumRenderTargets;

	_ASSERT(PSODesc.NumRenderTargets <= 8);
	for (size_t iRT = 0; iRT < _countof(PSODesc.RTVFormats); ++iRT)
		RTVFormats[iRT] = PSODesc.RTVFormats[iRT];

	DSVFormat = PSODesc.DSVFormat;
	SampleDesc = PSODesc.SampleDesc;
	NodeMask = PSODesc.NodeMask;
	Flags = PSODesc.Flags;

	return *this;
}

bool sVariableGraphicsPipelineStateDesc::operator==(const sVariableGraphicsPipelineStateDesc& Other) const {
	return memcmp(this, &Other, sizeof(sVariableGraphicsPipelineStateDesc)) == 0;
}

void sVariableGraphicsPipelineStateDesc::ExtractFrom(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& PSODesc) {
	operator=(PSODesc);
}

void sVariableGraphicsPipelineStateDesc::ApplyTo(D3D12_GRAPHICS_PIPELINE_STATE_DESC& PSODesc) const {
	PSODesc.BlendState = BlendState;
	PSODesc.SampleMask = SampleMask;
	PSODesc.RasterizerState = RasterizerState;
	PSODesc.DepthStencilState = DepthStencilState;
	PSODesc.IBStripCutValue = IBStripCutValue;
	PSODesc.PrimitiveTopologyType = PrimitiveTopologyType;
	PSODesc.NumRenderTargets = NumRenderTargets;

	_ASSERT(NumRenderTargets <= 8);
	for (size_t iRT = 0; iRT < _countof(RTVFormats); ++iRT)
		PSODesc.RTVFormats[iRT] = RTVFormats[iRT];

	PSODesc.DSVFormat = DSVFormat;
	PSODesc.SampleDesc = SampleDesc;
	PSODesc.NodeMask = NodeMask;
	PSODesc.Flags = Flags;
}

cGraphicsProgram::cGraphicsProgram() {

}

cGraphicsProgram::~cGraphicsProgram() {
	Unbuild();
}

bool cGraphicsProgram::Build() {
	return true;
}

void cGraphicsProgram::Unbuild() {
	for (auto& Pair : PSOMap) {
		cGraphicsPipelineStateObject* pPSO = Pair.second;
		delete pPSO;
	}
	PSOMap.clear();
}

const cGraphicsPipelineStateObject* cGraphicsProgram::GetPSO(const sVariableGraphicsPipelineStateDesc& Desc) const {
	auto Iter = PSOMap.find(Desc);
	if (Iter == PSOMap.end())
		return nullptr;

	return Iter->second;
}
