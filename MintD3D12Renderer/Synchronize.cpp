#include "pch.h"
#include "Synchronize.h"

#include "Device.h"

using namespace MintChoco;

cEvent::cEvent() {
	hEvent = nullptr;
}

cEvent::~cEvent() {
	Destroy();
}

bool cEvent::Create(bool InitialState, bool bManual) {
	if (IsCreated()) {
		return false;
	}
	UINT Flags = 0;
	Flags |= (InitialState	? CREATE_EVENT_INITIAL_SET :	0);
	Flags |= (bManual		? CREATE_EVENT_MANUAL_RESET :	0);
	hEvent = CreateEventEx(nullptr, nullptr, Flags, EVENT_ALL_ACCESS);
	return hEvent;
}

void cEvent::Destroy() {
	if (hEvent) {
		CloseHandle(hEvent);
		hEvent = nullptr;
	}
}

DWORD cEvent::Wait(DWORD Timeout) const {
	if (!IsCreated()) {
		return WAIT_FAILED;
	}

	return WaitForSingleObject(hEvent, Timeout);
}

bool cEvent::SetEvent(bool bSet) const {
	if (hEvent == nullptr)
		return false;

	if (bSet)
		return ::SetEvent(hEvent) == TRUE;
	else
		return ::ResetEvent(hEvent) == TRUE;

	return false;
}

cSRWLock::cSRWLock() {
#ifdef _DEBUG
	bInit = false;
#endif
}

cSRWLock::~cSRWLock() {
	Destroy();
}

bool cSRWLock::Create() {
	InitializeSRWLock(&Lock);
#ifdef _DEBUG
	bInit = true;
#endif

	return true;
}

void cSRWLock::Destroy() {
#ifdef _DEBUG
	bInit = false;
#endif
}

bool cSRWLock::LockExclusive() {
	assert(bInit);
	AcquireSRWLockExclusive(&Lock);

	return true;
}

void cSRWLock::UnlockExclusive() {
	assert(bInit);
	ReleaseSRWLockExclusive(&Lock);
}

bool cSRWLock::LockShared() {
	assert(bInit);
	AcquireSRWLockShared(&Lock);

	return true;
}

void cSRWLock::UnlockShared() {
	assert(bInit);
	ReleaseSRWLockShared(&Lock);
}

cFence::cFence() {

}

cFence::~cFence() {
	Destroy();
}

bool cFence::Create(const cDevice& Device, UINT InitialValue, D3D12_FENCE_FLAGS Flags) {
	if (!Device.IsCreated()) {
		assert(false);
		return false;
	}

	auto Result = Device.GetDevice()->CreateFence(InitialValue, Flags, IID_PPV_ARGS(FencePtr.ReleaseAndGetAddressOf()));
	if (Result != S_OK) {
		assert(false);
		return false;
	}

	return true;
}

void cFence::Destroy() {
	FencePtr.Reset();
}

std::optional<UINT64> cFence::GetValue() const {
	if (!IsCreated()) {
		assert(false);
		return std::nullopt;
	}

	return FencePtr->GetCompletedValue();
}

bool cFence::SetValue(UINT64 Value) {
	if (!IsCreated()) {
		assert(false);
		return false;
	}

	return FencePtr->Signal(Value) == S_OK;
}

bool cFence::SetEventOn(UINT64 Value, const cEvent& Event) {
	if (!IsCreated() || !Event.IsCreated()) {
		assert(false);
		return false;
	}

	return FencePtr->SetEventOnCompletion(Value, Event.GetEvent()) == S_OK;
}

cFrameSync::cFrameSync() {
	iFrame = 0;
}

cFrameSync::~cFrameSync() {
	Destroy();
}

bool cFrameSync::Create(cDevice& Device, size_t _BackBufferCount) {
	BackBufferCount = _BackBufferCount;
	if (!Fence.Create(Device, static_cast<UINT>(iFrame), D3D12_FENCE_FLAG_NONE))
		return false;

	if (!Event.Create(false, false)) {
		Destroy();
		return false;
	}

	for (size_t iBuffer = 0; iBuffer < BackBufferCount; ++iBuffer) {
		SignaledFrames[iBuffer] = 0;
		if (!GraphicsAllocators[iBuffer].Create(Device, D3D12_COMMAND_LIST_TYPE_DIRECT)) {
			Destroy();
			return false;
		}
		if (!GraphicsCommandLists[iBuffer].Create(Device, GraphicsAllocators[iBuffer], D3D12_COMMAND_LIST_TYPE_DIRECT)) {
			Destroy();
			return false;
		}
	}

	return true;
}

void cFrameSync::Destroy() {
	WaitAllBuffer();
	for (size_t iBuffer = 0; iBuffer < cSwapchain::MAX_BUFFER_COUNT; ++iBuffer) {
		SignaledFrames[iBuffer] = 0;
		GraphicsCommandLists[iBuffer].Destroy();
		GraphicsAllocators[iBuffer].Destroy();
	}
	BackBufferCount = 0;

	Event.Destroy();
	Fence.Destroy();
}

void cFrameSync::Signal(cCommandQueue& CommandQueue, size_t iBackBuffer) {
	if (!CommandQueue.IsCreated() || !Fence.IsCreated())
		return;

	if (BackBufferCount <= iBackBuffer)
		return;

	CommandQueue.Signal(Fence, static_cast<UINT>(++iFrame));
	SignaledFrames[iBackBuffer] = iFrame;
}

void cFrameSync::WaitBackBuffer(size_t iBackBuffer) {
	if (BackBufferCount <= iBackBuffer)
		return;

	if (!Fence.IsCreated() || !Event.IsCreated())
		return;

	size_t ValueToWait = SignaledFrames[iBackBuffer];
	if (Fence.GetValue() < ValueToWait) {
		Fence.SetEventOn(ValueToWait, Event);
		Event.Wait(INFINITE);
	}
}

void cFrameSync::WaitAllBuffer() {
	if (!Fence.IsCreated() || !Event.IsCreated())
		return;

	size_t ValueToWait = iFrame;

	if (Fence.GetValue() < ValueToWait) {
		Fence.SetEventOn(ValueToWait, Event);
		Event.Wait(INFINITE);
	}
}

void cFrameSync::InitGraphicsCommandList(size_t iBackBuffer) {
	assert(iBackBuffer < BackBufferCount);
	GraphicsAllocators[iBackBuffer].Reset();
	GraphicsCommandLists[iBackBuffer].Reset(GraphicsAllocators[iBackBuffer]);
}

cGraphicsCommandList& cFrameSync::GetGraphicsCommandList(size_t iBackBuffer) {
	assert(iBackBuffer < BackBufferCount);

	return GraphicsCommandLists[iBackBuffer];
}
