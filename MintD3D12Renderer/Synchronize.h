#pragma once

#include <optional>
#include <wrl.h>

#include "Command.h"
#include "Swapchain.h"
#include "Utilities.h"

namespace MintChoco {
	using Microsoft::WRL::ComPtr;

	class cEvent {
		DELETE_CLASS_COPY(cEvent);
	public:
		cEvent();
		~cEvent();

		bool Create(bool InitialState, bool bManual);
		void Destroy();

		bool	IsCreated() const { return hEvent; }

		DWORD	Wait(DWORD Timeout) const;

		HANDLE	GetEvent() const { return hEvent; }

		bool	SetEvent(bool bSet) const;

	protected:
		HANDLE hEvent;
	};

	class cDevice;
	class cFence {
		DELETE_CLASS_COPY(cFence);
	public:
		using ID3D12FenceN = ID3D12Fence;

		cFence();
		~cFence();

		bool Create(const cDevice& Device, UINT InitialValue, D3D12_FENCE_FLAGS Flags);
		void Destroy();

		bool IsCreated() const { return FencePtr.Get(); }

		ID3D12FenceN*			GetFence() const { return FencePtr.Get(); }

		std::optional<UINT64>	GetValue() const;
		bool	SetValue(UINT64 Value);
		bool	SetEventOn(UINT64 Value, const cEvent& Event);
	protected:
		ComPtr<ID3D12FenceN> FencePtr;
	};

	class cFrameSync {
	public:
		cFrameSync();
		~cFrameSync();

		bool Create(cDevice& Device, size_t BackBufferCount);
		void Destroy();

		void Signal(cCommandQueue& CommandQueue, size_t iBackBuffer);
		void WaitBackBuffer(size_t iBackBuffer);
		void WaitAllBuffer();

		void					InitGraphicsCommandList(size_t iBackBuffer);
		cGraphicsCommandList&	GetGraphicsCommandList(size_t iBackBuffer);
	protected:
		cFence Fence;
		cEvent Event;

		size_t iFrame;

		size_t BackBufferCount;
		std::array<size_t,					cSwapchain::MAX_BUFFER_COUNT>	SignaledFrames;
		std::array<cCommandAllocator,		cSwapchain::MAX_BUFFER_COUNT>	GraphicsAllocators;
		std::array<cGraphicsCommandList,	cSwapchain::MAX_BUFFER_COUNT>	GraphicsCommandLists;
	};		  
}
