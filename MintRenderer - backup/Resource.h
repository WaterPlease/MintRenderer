#pragma once

#include <optional>
#include <wrl.h>

#include "Geometry.h"

namespace MintChoco {
	class cDevice;
	using Microsoft::WRL::ComPtr;

	class cResource {
	public:
		using ID3D12ResourceN = ID3D12Resource2;
		struct sMMapToken {
			size_t Offset;
			size_t Size;
			void* Ptr;

			sMMapToken();
			operator D3D12_RANGE() const;
		};

		cResource();
		~cResource();

		bool Create(ID3D12Resource* pResource);
		void Destroy();

		sMMapToken	Map(size_t Offset, size_t Size);
		void		Unmap(const sMMapToken& Token);

		bool					IsCreated() const { return ResourcePtr.Get(); }
		ID3D12ResourceN*		GetResouce() const { return ResourcePtr.Get(); };

		void								InvalidateDescription();
		const D3D12_RESOURCE_DESC1&			GetDescription1() const;
	protected:
		ComPtr<ID3D12ResourceN> ResourcePtr;

		bool bDescriptionInvalid;
		D3D12_RESOURCE_DESC1 Desc1;
	};

	class cResoureBarrier {
	public:
		cResoureBarrier();
		~cResoureBarrier();

		bool CreateTransition(const cResource& Resource, D3D12_RESOURCE_STATES StateBefore, D3D12_RESOURCE_STATES StateAfter, bool bTransitSubresource, D3D12_RESOURCE_BARRIER_FLAGS Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
		bool CreateAliasing(const cResource& ResourceBefore, cResource& ResourceAfter, D3D12_RESOURCE_BARRIER_FLAGS Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);
		bool CreateUAV(const cResource& Resource, D3D12_RESOURCE_BARRIER_FLAGS Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

		bool IsCreated() const { return bCreated; }

		D3D12_RESOURCE_BARRIER* GetBarrier() { return &Barrier; }
		const D3D12_RESOURCE_BARRIER* GetBarrier() const { return &Barrier; }
	protected:
		bool bCreated;
		D3D12_RESOURCE_BARRIER Barrier;
	};

	class cResourceFactory {
	public:
		cResourceFactory(cDevice& Device) : Device(Device) {}

		bool CreateBuffer(cResource& Resource, size_t BufferSize, D3D12_HEAP_FLAGS HeapFlag, D3D12_RESOURCE_FLAGS ResourceFlag, D3D12_RESOURCE_STATES InitailState= D3D12_RESOURCE_STATE_COMMON);
		bool CreateUploadBuffer(cResource& Resource, size_t BufferSize);

		bool CreateTextureBuffer(cResource& Resource, const cVolumeui& TextureSize, bool bArray, DXGI_FORMAT Format, D3D12_TEXTURE_LAYOUT Layout, D3D12_HEAP_FLAGS HeapFlag, D3D12_RESOURCE_FLAGS ResourceFlag, D3D12_RESOURCE_STATES InitailState = D3D12_RESOURCE_STATE_COMMON);
	protected:
		cDevice& Device;
	};
}
