#include "pch.h"
#include "Resource.h"

#include "Device.h"
#include "Utilities.h"
#include <algorithm>

using namespace MintChoco;

cResource::sMMapToken::sMMapToken()
: Offset(0), Size(0), Ptr(nullptr) {
	
}

cResource::sMMapToken::operator D3D12_RANGE() const {
	return D3D12_RANGE{ Offset, Offset + Size };
}

cResource::cResource() {
	InvalidateDescription();
}

cResource::~cResource() {
	Destroy();
}

bool cResource::Create(ID3D12Resource* pResource) {
	if (pResource == nullptr)
		return false;

	if (pResource->QueryInterface(IID_PPV_ARGS(ResourcePtr.ReleaseAndGetAddressOf())) != S_OK)
		return false;

	InvalidateDescription();
	GetDescription1();

	return true;
}

void cResource::Destroy() {
	ResourcePtr.Reset();
	InvalidateDescription();
}

cResource::sMMapToken cResource::Map(size_t Offset, size_t Size) {
	if (!IsCreated())
		return sMMapToken();

	sMMapToken Token;
	Token.Offset = Offset;
	Token.Size = Size;

	D3D12_RANGE Range = Token;
	if (ResourcePtr->Map(0, &Range, &Token.Ptr) != S_OK)
		Token.Ptr = nullptr;

	return Token;
}

void cResource::Unmap(sMMapToken& Token) {
	if (!IsCreated())
		return;

	if (Token.Ptr == nullptr)
		return;

	D3D12_RANGE Range = Token;
	ResourcePtr->Unmap(0, &Range);

	Token = {};
}

void cResource::InvalidateDescription() {
	bDescriptionInvalid = true;
}

const D3D12_RESOURCE_DESC1& cResource::GetDescription1() const {
	if (bDescriptionInvalid) {
		const_cast<cResource*>(this)->Desc1 = GetResouce()->GetDesc1();
		const_cast<cResource*>(this)->bDescriptionInvalid = false;
	}

	return Desc1;
}

cResoureBarrier::cResoureBarrier() {
	bCreated = false;
}

cResoureBarrier::~cResoureBarrier() {

}

bool cResoureBarrier::CreateTransition(const cResource& Resource, D3D12_RESOURCE_STATES StateBefore,
	D3D12_RESOURCE_STATES StateAfter, bool bTransitSubresource, D3D12_RESOURCE_BARRIER_FLAGS Flags) {
	bCreated = false;

	Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	Barrier.Flags = Flags;

	if (!Resource.GetResouce())
		return false;

	Barrier.Transition.pResource = Resource.GetResouce();
	Barrier.Transition.Subresource = bTransitSubresource ? 0xffffffff : 0;
	Barrier.Transition.StateBefore = StateBefore;
	Barrier.Transition.StateAfter = StateAfter;

	bCreated = true;
	return true;
}

bool cResoureBarrier::CreateAliasing(const cResource& ResourceBefore, cResource& ResourceAfter, D3D12_RESOURCE_BARRIER_FLAGS Flags) {
	bCreated = false;

	Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
	Barrier.Flags = Flags;

	if (!ResourceBefore.GetResouce() || !ResourceAfter.GetResouce())
		return false;

	Barrier.Aliasing.pResourceBefore = ResourceBefore.GetResouce();
	Barrier.Aliasing.pResourceAfter = ResourceAfter.GetResouce();

	bCreated = true;
	return true;
}

bool cResoureBarrier::CreateUAV(const cResource& Resource, D3D12_RESOURCE_BARRIER_FLAGS Flags) {
	bCreated = false;

	Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	Barrier.Flags = Flags;

	if (!Resource.GetResouce())
		return false;

	Barrier.UAV.pResource = Resource.GetResouce();

	bCreated = true;
	return true;
}

bool cResourceFactory::CreateBuffer(cResource& Resource, size_t BufferSize, D3D12_HEAP_FLAGS HeapFlag, D3D12_RESOURCE_FLAGS ResourceFlag, D3D12_RESOURCE_STATES InitailState) {
	if (!Device.IsCreated())
		return false;
	if (Resource.IsCreated())
		return false;

	D3D12_HEAP_PROPERTIES HeapProperties;
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 0;
	HeapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC ResourceDescription;
	ResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	ResourceDescription.Width = BufferSize;
	ResourceDescription.Height = 1;
	ResourceDescription.DepthOrArraySize = 1;
	ResourceDescription.MipLevels = 1;
	ResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDescription.SampleDesc.Count = 1;
	ResourceDescription.SampleDesc.Quality = 0;
	ResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

	ComPtr<ID3D12Resource> ResourcePtr;
	auto Result = Device.GetDevice()->CreateCommittedResource(
		&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDescription, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(ResourcePtr.ReleaseAndGetAddressOf())
	);

	if (Result != S_OK)
		return false;

	return Resource.Create(ResourcePtr.Get());
}

bool cResourceFactory::CreateUploadBuffer(cResource& Resource, size_t BufferSize) {
	if (!Device.IsCreated())
		return false;
	if (Resource.IsCreated())
		return false;

	D3D12_HEAP_PROPERTIES HeapProperties;
	HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 0;
	HeapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC ResourceDescription;
	ResourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	ResourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	ResourceDescription.Width = BufferSize;
	ResourceDescription.Height = 1;
	ResourceDescription.DepthOrArraySize = 1;
	ResourceDescription.MipLevels = 1;
	ResourceDescription.Format = DXGI_FORMAT_UNKNOWN;
	ResourceDescription.SampleDesc.Count = 1;
	ResourceDescription.SampleDesc.Quality = 0;
	ResourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ResourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

	ComPtr<ID3D12Resource> ResourcePtr;
	auto Result = Device.GetDevice()->CreateCommittedResource(
		&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDescription, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(ResourcePtr.ReleaseAndGetAddressOf())
	);

	if (Result != S_OK)
		return false;

	return Resource.Create(ResourcePtr.Get());
}

bool cResourceFactory::CreateTextureBuffer(cResource& Resource, const cVolumeui& TextureSize, bool bArray, DXGI_FORMAT Format, D3D12_TEXTURE_LAYOUT Layout, D3D12_HEAP_FLAGS HeapFlag,
	D3D12_RESOURCE_FLAGS ResourceFlag, D3D12_RESOURCE_STATES InitailState) {
	if (!Device.IsCreated())
		return false;
	if (Resource.IsCreated())
		return false;

	D3D12_HEAP_PROPERTIES HeapProperties;
	HeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	HeapProperties.CreationNodeMask = 0;
	HeapProperties.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC ResourceDesc;
	if (!bArray) {
		if (TextureSize.X == 0) {
			return false;
		}

		if (TextureSize.Y == 0) {
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		}
		else if (TextureSize.Z == 0) {
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		}
		else {
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		}
	}
	else {
		if (TextureSize.X == 0 || TextureSize.Z == 0) {
			return false;
		}

		if (TextureSize.Y == 0) {
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		}
		else {
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		}
		
	}
	ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	ResourceDesc.Width = std::max(TextureSize.X, static_cast<UINT>(1));
	ResourceDesc.Height = std::max(TextureSize.Y, static_cast<UINT>(1));
	ResourceDesc.DepthOrArraySize = std::max(TextureSize.Z, static_cast<UINT>(1));
	ResourceDesc.MipLevels = 1;
	ResourceDesc.Format = Format;
	ResourceDesc.SampleDesc = {1, 0};
	ResourceDesc.Layout = Layout;
	ResourceDesc.Flags = ResourceFlag;

	ComPtr<ID3D12Resource> ResourcePtr;
	auto Result = Device.GetDevice()->CreateCommittedResource(
		&HeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc, InitailState, nullptr, IID_PPV_ARGS(ResourcePtr.ReleaseAndGetAddressOf())
	);

	if (Result != S_OK)
		return false;

	return Resource.Create(ResourcePtr.Get());
}