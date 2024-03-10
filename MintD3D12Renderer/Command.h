#pragma once

#include <vector>
#include <wrl.h>

#include "Utilities.h"
#include "DirectXMath.h"

namespace MintChoco {
	class cDescriptorHeap;
	class cResource;
	using Microsoft::WRL::ComPtr;

	class cDevice;
	class cFence;
	class cCommandList;
	class cCommandQueue {
		DELETE_CLASS_COPY(cCommandQueue);
	public:
		using ID3D12CommandQueueN = ID3D12CommandQueue;
		constexpr static size_t TypeCount = D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE + 1;

		cCommandQueue();
		~cCommandQueue();

		bool Create(const cDevice& Device,D3D12_COMMAND_LIST_TYPE Type, D3D12_COMMAND_QUEUE_PRIORITY Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL, D3D12_COMMAND_QUEUE_FLAGS Flags = D3D12_COMMAND_QUEUE_FLAG_NONE, UINT NodeMask = 0);
		void Destroy();

		bool IsCreated() const { return CommandQueuePtr.Get() != nullptr; }

		ID3D12CommandQueueN*			GetCommandQueue() const { return CommandQueuePtr.Get(); };

		D3D12_COMMAND_LIST_TYPE			GetType()		const { return Type; }
		D3D12_COMMAND_QUEUE_PRIORITY	GetPriority()	const { return Priority; }
		D3D12_COMMAND_QUEUE_FLAGS		GetFlags()		const { return Flags; }
		UINT							GetNodeMask()	const { return NodeMask; }

		bool Signal(cFence& Fence, UINT Value) const;

		bool Sync(const cDevice& Device, DWORD Timeout) const;

		void ExecuteCommandList(cCommandList& CommandList);
		void ExecuteCommandList(std::vector<cCommandList*> CommandLists);
		void ExecuteCommandList(size_t NumCommandLists, ID3D12CommandList** ppCommandLists);
	protected:
		ComPtr<ID3D12CommandQueueN> CommandQueuePtr;

		D3D12_COMMAND_LIST_TYPE Type;
		D3D12_COMMAND_QUEUE_PRIORITY Priority;
		D3D12_COMMAND_QUEUE_FLAGS Flags;
		UINT NodeMask;
	};

	class cCommandAllocator {
		DELETE_CLASS_COPY(cCommandAllocator);
	public:
		using ID3D12CommandAllocatorN = ID3D12CommandAllocator;

		cCommandAllocator();
		~cCommandAllocator();

		bool Create(const cDevice& Device, D3D12_COMMAND_LIST_TYPE Type);
		void Destroy();

		bool Reset();

		bool IsCreated() const { return CommandAllocator.Get(); }

		ID3D12CommandAllocatorN* GetCommandAllocator() const { return CommandAllocator.Get(); }
	protected:
		ComPtr<ID3D12CommandAllocatorN> CommandAllocator;
		D3D12_COMMAND_LIST_TYPE Type;
	};

	// DO NOT USE
	class cCommandList {
		DELETE_CLASS_COPY(cCommandList);
	public:
		cCommandList();

		bool IsOpened() const { return bOpen; }
		bool IsClosed() const { return !bOpen; }

		virtual ID3D12CommandList* GetCommandList() const { return NULL; }
	protected:
		D3D12_COMMAND_LIST_TYPE		Type;
		bool						bOpen;
	};

	class cGraphicsCommandList : public cCommandList {
		DELETE_CLASS_COPY(cGraphicsCommandList);
	public:
		using ID3D12GraphicsCommandListN = ID3D12GraphicsCommandList7;

		cGraphicsCommandList();
		~cGraphicsCommandList();

		// 추후 PipeState 클래스 생성 시에 메소드 시그니처 수정하기
		bool Create(const cDevice& Device, cCommandAllocator& Allocator, D3D12_COMMAND_LIST_TYPE Type, UINT NodeMask = 0, ID3D12PipelineState* pPipeState = NULL);
		void Destroy();

		bool IsCreated() const { return CommandListPtr.Get(); }

		ID3D12GraphicsCommandListN* GetGraphicsCommandList() const { return CommandListPtr.Get(); }
		virtual ID3D12CommandList* GetCommandList() const override { return CommandListPtr.Get(); }

		// ID3D12GraphicsCommandList 인터페이스는 여기에 추가
		bool Reset(cCommandAllocator& Allocator, ID3D12PipelineState* pPipeState = NULL);
		bool Close();

		void OMSetRenderTargetsContinuous(size_t RTCount, const D3D12_CPU_DESCRIPTOR_HANDLE* pRTDescriptorHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* pDSDescriptorHandle);
		void OMSetRenderTargetsDiscrete(size_t RTCount, const D3D12_CPU_DESCRIPTOR_HANDLE* pRTDescriptorHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* pDSDescriptorHandle);

		void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const float* Color4f, size_t RectCount, const D3D12_RECT* pRect);

		void CopyResource(const cResource& Dest, const cResource& Source);
		void CopyBufferRegion(const cResource& Dest, size_t DestOffset, const cResource& Source, size_t SourceOffset, size_t Size);

		void SetDescriptorHeap(const cDescriptorHeap& DescriptorHeap);
		void SetDescriptorHeaps(const std::vector<const cDescriptorHeap*>& DescriptorHeaps);

		void ResourceTransition(const cResource& Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter);
	protected:
		ComPtr<ID3D12GraphicsCommandListN>	CommandListPtr;
	};
}

