#pragma once

#include <vector>
#include <wrl.h>

#include "Utilities.h"

namespace MintChoco {
	class cResource;
	class cDevice;
	using Microsoft::WRL::ComPtr;

	class cDescriptorHeap {
		DELETE_CLASS_COPY(cDescriptorHeap)
	public:
		using ID3D12DescriptorHeapN = ID3D12DescriptorHeap;
		cDescriptorHeap();
		~cDescriptorHeap();

		bool Create(cDevice& Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, size_t DescriptorCount, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, UINT NodeMask = 0);
		void Destroy();

		// Descriptor Creation
		bool CreateRenderTargetView(cDevice& Device, cResource& Resource, size_t iDescriptor, const D3D12_RENDER_TARGET_VIEW_DESC& Desc) const;
		bool CreateShaderResourceView(cDevice& Device, cResource& Resource, size_t iDescriptor, const D3D12_SHADER_RESOURCE_VIEW_DESC& Desc) const;
		bool CreateConstantBufferView(cDevice& Device, size_t iDescriptor, const D3D12_CONSTANT_BUFFER_VIEW_DESC& Desc) const;
		bool CreateSampler(cDevice& Device, size_t iDescriptor, const D3D12_SAMPLER_DESC& Desc) const;

		bool					IsCreated() const { return DescriptorHeap.Get(); }
		ID3D12DescriptorHeapN*	GetDescriptorHeap() const { return DescriptorHeap.Get(); }

		const D3D12_CPU_DESCRIPTOR_HANDLE* GetCPUDescriptorHandle(size_t iDescriptor) const;
		const D3D12_GPU_DESCRIPTOR_HANDLE* GetGPUDescriptorHandle(size_t iDescriptor) const;
	protected:
		ComPtr<ID3D12DescriptorHeapN> DescriptorHeap;

		D3D12_DESCRIPTOR_HEAP_TYPE Type;
		size_t DescriptorCount;
		bool bIsShaderVisible;

		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> CPUDescriptorHandle;
		std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> GPUDescriptorHandle;
	};

	class cRootSignature {
		DELETE_CLASS_COPY(cRootSignature)
	public:
		using ID3D12RootSignatureN = ID3D12RootSignature;
		cRootSignature();
		~cRootSignature();

		bool Create(cDevice& Device, D3D12_SHADER_BYTECODE ShaderByteCode, UINT NodeMask = 0);
		void Destroy();

		bool IsCreated() const { return RootSignaturePtr.Get(); }
		ID3D12RootSignatureN* GetRootSignature() const { return RootSignaturePtr.Get(); }

	protected:
		ComPtr<ID3D12RootSignatureN> RootSignaturePtr;
	};
}

