#include "pch.h"
#include "Command.h"

#include <algorithm>

#include "Device.h"
#include "Synchronize.h"

using namespace MintChoco;

cCommandQueue::cCommandQueue() {

}

cCommandQueue::~cCommandQueue() {
	Destroy();
}

bool cCommandQueue::Create(const cDevice& Device, D3D12_COMMAND_LIST_TYPE _Type,
	D3D12_COMMAND_QUEUE_PRIORITY _Priority, D3D12_COMMAND_QUEUE_FLAGS _Flags, UINT _NodeMask) {
	if (!Device.IsCreated()) {
		assert(false);
		return false;
	}

	if (IsCreated()) {
		assert(false);
		return false;
	}
	D3D12_COMMAND_QUEUE_DESC Desc;
	Desc.Type = _Type;
	Desc.Priority = _Priority;
	Desc.Flags = _Flags;
	Desc.NodeMask = _NodeMask;
	auto Result = Device.GetDevice()->CreateCommandQueue(&Desc, IID_PPV_ARGS(CommandQueuePtr.ReleaseAndGetAddressOf()));
	if (Result != S_OK || CommandQueuePtr.Get() == nullptr) {
		assert(false);
		return false;
	}

	Type = Desc.Type;
	Priority = static_cast<D3D12_COMMAND_QUEUE_PRIORITY>(Desc.Priority);
	Flags = Desc.Flags;
	NodeMask = Desc.NodeMask;
	return true;
}

void cCommandQueue::Destroy() {
	CommandQueuePtr.Reset();
}

bool cCommandQueue::Signal(cFence& Fence, UINT Value) const {
	if (!IsCreated() || !Fence.IsCreated()) {
		assert(false);
		return false;
	}

	return CommandQueuePtr->Signal(Fence.GetFence(), Value) == S_OK;
}

bool cCommandQueue::Sync(const cDevice& Device, DWORD Timeout) const {
	cFence Fence;
	if (!Fence.Create(Device, 0, D3D12_FENCE_FLAG_NONE)) {
		assert(false);
		return false;
	}

	cEvent Event;
	if (!Event.Create(false, true)) {
		assert(false);
		return false;
	}

	if (!Fence.SetEventOn(1, Event)) {
		assert(false);
		return false;
	}

	Signal(Fence, 1);
	return Event.Wait(Timeout) == WAIT_OBJECT_0;
}

void cCommandQueue::ExecuteCommandList(cCommandList& CommandList) {
	if (CommandList.IsOpened()) {
		assert(false);
		return;
	}

	ID3D12CommandList* pCommandList = CommandList.GetCommandList();
	ExecuteCommandList(1, &pCommandList);
}

void cCommandQueue::ExecuteCommandList(std::vector<cCommandList*> CommandLists) {
	std::vector<ID3D12CommandList*> D3D12CommandList(CommandLists.size());
	for (size_t iCommandList = 0; iCommandList < CommandLists.size(); ++iCommandList) {
		if (!CommandLists[iCommandList]->IsOpened()) {
			assert(false);
			return;
		}
		D3D12CommandList[iCommandList] = CommandLists[iCommandList]->GetCommandList();
	}
	ExecuteCommandList(CommandLists.size(), D3D12CommandList.data());
}

void cCommandQueue::ExecuteCommandList(size_t NumCommandLists, ID3D12CommandList** ppCommandLists) {
	ASSERT_RETURN_IF_NOT_CREATED();

	if (ppCommandLists == nullptr) {
		assert(false);
		return;
	}
	for (size_t iList = 0; iList < NumCommandLists; ++iList) {
		if (ppCommandLists[iList] == nullptr) {
			assert(false);
			return;
		}
	}

	CommandQueuePtr->ExecuteCommandLists(static_cast<UINT>(NumCommandLists), ppCommandLists);
}

cCommandAllocator::cCommandAllocator() {

}

cCommandAllocator::~cCommandAllocator() {
	Destroy();
}

bool cCommandAllocator::Create(const cDevice& Device, D3D12_COMMAND_LIST_TYPE _Type) {
	if (!Device.IsCreated()) {
		assert(false);
		return false;
	}

	if (IsCreated()) {
		assert(false);
		return false;
	}

	auto Result = Device.GetDevice()->CreateCommandAllocator(_Type, IID_PPV_ARGS(CommandAllocator.ReleaseAndGetAddressOf()));
	if (Result != S_OK) {
		return false;
	}

	Type = _Type;
	return true;
}

void cCommandAllocator::Destroy() {
	CommandAllocator.Reset();
}

bool cCommandAllocator::Reset() {
	return CommandAllocator.Get()->Reset() == S_OK;
}

cCommandList::cCommandList() {
	bOpen = false;
}

cGraphicsCommandList::cGraphicsCommandList() {

}

cGraphicsCommandList::~cGraphicsCommandList() {
	Destroy();
}

bool cGraphicsCommandList::Create(const cDevice& Device, cCommandAllocator& Allocator, D3D12_COMMAND_LIST_TYPE _Type, UINT NodeMask, ID3D12PipelineState* pPipeState) {
	if (!Device.IsCreated() || !Allocator.IsCreated()) {
		assert(false);
		return false;
	}

	ASSERT_RETURN_FALSE_IF_CREATED();

	auto Result = Device.GetDevice()->CreateCommandList(
		NodeMask,
		_Type,
		Allocator.GetCommandAllocator(),
		pPipeState,
		IID_PPV_ARGS(CommandListPtr.ReleaseAndGetAddressOf())
	);
	if (Result != S_OK) {
		return false;
	}

	Type = _Type;
	bOpen = false; // Enforce ::Reset
	Close();

	return true;
}

void cGraphicsCommandList::Destroy() {
	CommandListPtr.Reset();
	bOpen = false;
}

bool cGraphicsCommandList::Reset(cCommandAllocator& Allocator, ID3D12PipelineState* pPipeState) {
	if (!Allocator.IsCreated()) {
		assert(false);
		return false;
	}

	ASSERT_RETURN_FALSE_IF_NOT_CREATED();

	bool bResult = CommandListPtr->Reset(Allocator.GetCommandAllocator(), pPipeState) == S_OK;
	if (bResult) {
		bOpen = true;
	}
	else {
		assert(false);
	}

	return bResult;
}

bool cGraphicsCommandList::Close() {
	ASSERT_RETURN_FALSE_IF_NOT_CREATED();

	bool bResult = CommandListPtr->Close() == S_OK;
	if (bResult) {
		bOpen = false;
	}
	else {
		assert(false);
	}

	return bResult;
}

void cGraphicsCommandList::OMSetRenderTargetsContinuous(size_t RTCount,
	const D3D12_CPU_DESCRIPTOR_HANDLE* pRTDescriptorHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* pDSDescriptorHandle) {
	RETURN_IF_NOT_CREATED();

	CommandListPtr->OMSetRenderTargets(static_cast<UINT>(RTCount), pRTDescriptorHandle, TRUE, pDSDescriptorHandle);
}

void cGraphicsCommandList::OMSetRenderTargetsDiscrete(size_t RTCount,
	const D3D12_CPU_DESCRIPTOR_HANDLE* pRTDescriptorHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* pDSDescriptorHandle) {
	RETURN_IF_NOT_CREATED();

	CommandListPtr->OMSetRenderTargets(static_cast<UINT>(RTCount), pRTDescriptorHandle, FALSE, pDSDescriptorHandle);
}

void cGraphicsCommandList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView,
	const float* Color4f, size_t RectCount, const D3D12_RECT* pRect) {
	RETURN_IF_NOT_CREATED();

	CommandListPtr->ClearRenderTargetView(RenderTargetView, Color4f, static_cast<UINT>(RectCount), pRect);
}

void cGraphicsCommandList::CopyResource(const cResource& Dest, const cResource& Source) {
	RETURN_IF_NOT_CREATED();

	if (!Dest.IsCreated() || !Source.IsCreated())
		return;

	CommandListPtr->CopyResource(Dest.GetResouce(), Source.GetResouce());
}

void cGraphicsCommandList::CopyBufferRegion(const cResource& Dest, size_t DestOffset, const cResource& Source,
	size_t SourceOffset, size_t Size) {
	RETURN_IF_NOT_CREATED();

	if (!Dest.IsCreated() || !Source.IsCreated())
		return;

	CommandListPtr->CopyBufferRegion(Dest.GetResouce(), DestOffset, Source.GetResouce(), SourceOffset, Size);
}

void cGraphicsCommandList::SetDescriptorHeap(const cDescriptorHeap& DescriptorHeap) {
	std::vector<const cDescriptorHeap*> Heaps(1);
	Heaps[0] = &DescriptorHeap;
	SetDescriptorHeaps(Heaps);
}

void cGraphicsCommandList::SetDescriptorHeaps(const std::vector<const cDescriptorHeap*>& DescriptorHeaps) {
	std::vector<ID3D12DescriptorHeap*> HeapPtrs(DescriptorHeaps.size());
	std::transform(DescriptorHeaps.begin(), DescriptorHeaps.end(), HeapPtrs.begin(),
		[](const cDescriptorHeap* pHeap) { return pHeap->GetDescriptorHeap(); });

	CommandListPtr->SetDescriptorHeaps(static_cast<UINT>(HeapPtrs.size()), HeapPtrs.data());
}

void cGraphicsCommandList::ResourceTransition(const cResource& Resource, D3D12_RESOURCE_STATES StateBefore,
	D3D12_RESOURCE_STATES StateAfter) {
	cResoureBarrier Barrier;
	if (!Barrier.CreateTransition(Resource, StateBefore, StateAfter, false)) {
		assert(false);
		return;
	}

	CommandListPtr->ResourceBarrier(1, Barrier.GetBarrier());
}
