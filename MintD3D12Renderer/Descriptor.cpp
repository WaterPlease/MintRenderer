#include "pch.h"
#include "Descriptor.h"

#include "Utilities.h"
#include "Device.h"
#include "Resource.h"

using namespace MintChoco;

cDescriptorHeap::cDescriptorHeap() {
	bIsShaderVisible = false;
}

cDescriptorHeap::~cDescriptorHeap() {

}

bool cDescriptorHeap::Create(cDevice& Device, D3D12_DESCRIPTOR_HEAP_TYPE _Type, size_t _DescriptorCount, D3D12_DESCRIPTOR_HEAP_FLAGS Flags,
	UINT NodeMask) {
	RETURN_FALSE_IF_CREATED();
	if (!Device.IsCreated())
		return false;

	D3D12_DESCRIPTOR_HEAP_DESC Desc;
	Desc.Type = _Type;
	Desc.NumDescriptors = static_cast<UINT>(_DescriptorCount);
	Desc.Flags = Flags;
	Desc.NodeMask = NodeMask;
	if (Device.GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(DescriptorHeap.ReleaseAndGetAddressOf())) != S_OK) {
		return false;
	}

	Type = _Type;
	DescriptorCount = _DescriptorCount;
	bIsShaderVisible = Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	CPUDescriptorHandle.resize(DescriptorCount);
	GPUDescriptorHandle.resize(DescriptorCount);
	D3D12_CPU_DESCRIPTOR_HANDLE FirstCPUHandle = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE FirstGPUHandle;
	if (bIsShaderVisible)
		FirstGPUHandle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	else
		FirstGPUHandle.ptr = NULL;

	size_t DescriptorSize = Device.GetDevice()->GetDescriptorHandleIncrementSize(Type);
	for (size_t iDescriptor = 0; iDescriptor < DescriptorCount; ++iDescriptor) {
		CPUDescriptorHandle[iDescriptor] = FirstCPUHandle;
		CPUDescriptorHandle[iDescriptor].ptr += iDescriptor * DescriptorSize;

		if (bIsShaderVisible) {
			GPUDescriptorHandle[iDescriptor] = FirstGPUHandle;
			GPUDescriptorHandle[iDescriptor].ptr += iDescriptor * DescriptorSize;
		}
	}

	return true;
}

void cDescriptorHeap::Destroy() {
	DescriptorHeap.Reset();
	bIsShaderVisible = false;
	DescriptorCount = 0;
	CPUDescriptorHandle.clear();
	GPUDescriptorHandle.clear();
}

bool cDescriptorHeap::CreateRenderTargetView(cDevice& Device, cResource& Resource, size_t iDescriptor, const D3D12_RENDER_TARGET_VIEW_DESC& Desc) {
	RETURN_FALSE_IF_NOT_CREATED();
	if (!Device.IsCreated())
		return false;

	if (Type != D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		return false;

	if (DescriptorCount <= iDescriptor)
		return false;

	if (!Resource.IsCreated())
		return false;

	const D3D12_CPU_DESCRIPTOR_HANDLE* DescriptorHandle = GetCPUDescriptorHandle(iDescriptor);
	if (DescriptorHandle == nullptr)
		return false;

	Device.GetDevice()->CreateRenderTargetView(Resource.GetResouce(), &Desc, *DescriptorHandle);
	return true;
}

bool cDescriptorHeap::CreateShaderResourceView(cDevice& Device, cResource& Resource, size_t iDescriptor,
	const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc) {
	RETURN_FALSE_IF_NOT_CREATED();
	if (!Device.IsCreated())
		return false;

	if (Type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		return false;

	if (DescriptorCount <= iDescriptor)
		return false;

	if (!Resource.IsCreated())
		return false;

	const D3D12_CPU_DESCRIPTOR_HANDLE* DescriptorHandle = GetCPUDescriptorHandle(iDescriptor);
	if (DescriptorHandle == nullptr)
		return false;

	Device.GetDevice()->CreateShaderResourceView(Resource.GetResouce(), &Desc, *DescriptorHandle);
	return true;
}

bool cDescriptorHeap::CreateConstantBufferView(cDevice& Device, size_t iDescriptor,
	const D3D12_CONSTANT_BUFFER_VIEW_DESC& Desc) {
	RETURN_FALSE_IF_NOT_CREATED();
	if (!Device.IsCreated())
		return false;

	if (Type != D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		return false;

	if (DescriptorCount <= iDescriptor)
		return false;

	const D3D12_CPU_DESCRIPTOR_HANDLE* DescriptorHandle = GetCPUDescriptorHandle(iDescriptor);
	if (DescriptorHandle == nullptr)
		return false;

	Device.GetDevice()->CreateConstantBufferView(&Desc, *DescriptorHandle);
	return true;
}

const D3D12_CPU_DESCRIPTOR_HANDLE* cDescriptorHeap::GetCPUDescriptorHandle(size_t iDescriptor) const {
	if (DescriptorCount <= iDescriptor)
		return nullptr;

	return &CPUDescriptorHandle[iDescriptor];
}

const D3D12_GPU_DESCRIPTOR_HANDLE* cDescriptorHeap::GetGPUDescriptorHandle(size_t iDescriptor) const {
	if (DescriptorCount <= iDescriptor)
		return  nullptr;

	if (!bIsShaderVisible)
		return nullptr;

	return &GPUDescriptorHandle[iDescriptor];
}

cRootSignature::cRootSignature() {

}

cRootSignature::~cRootSignature() {
	Destroy();
}

bool cRootSignature::Create(cDevice& Device, D3D12_SHADER_BYTECODE ShaderByteCode, UINT NodeMask) {
	auto Result = Device.GetDevice()->CreateRootSignature(NodeMask, ShaderByteCode.pShaderBytecode, ShaderByteCode.BytecodeLength, IID_PPV_ARGS(RootSignaturePtr.ReleaseAndGetAddressOf()));
	if (Result != S_OK)
		return false;

	return true;
}

void cRootSignature::Destroy() {
	RootSignaturePtr.Reset();
}
