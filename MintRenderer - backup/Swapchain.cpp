#include "pch.h"
#include "Swapchain.h"

#include "Renderer.h"
#include "HardwareManager.h"

using namespace MintChoco;

cSwapchainBuilder::cSwapchainBuilder(cBaseD3D12Renderer& Renderer)
	: Renderer(Renderer) {
	Clear();
}

cSwapchainBuilder::~cSwapchainBuilder() {

}

void cSwapchainBuilder::Clear() {
	InterfaceVersion = eInterfaceVersion::INTERFACE_VERSION_COUNT;
	SurfaceType = eSurfaceType::SURFACE_TYPE_COUNT;

	hWnd = std::nullopt;

	Size = std::nullopt;
	Format = std::nullopt;
	bStereo = std::nullopt;
	SampleCount = std::nullopt;
	BufferUsage = std::nullopt;
	BufferCount = std::nullopt;
	Scaling = std::nullopt;
	SwapEffect = std::nullopt;
	AlphaMode = std::nullopt;
	Flags = std::nullopt;
}

bool cSwapchainBuilder::Build(cSwapchain& Swapchain) {
	if (!Renderer.GetHWMngr().Initialize() || Renderer.GetHWMngr().GetDXGIFactory() == nullptr)
		return false;

	if (!Renderer.GetDevice().IsCreated())
		return false;

	switch(InterfaceVersion) {
	case eInterfaceVersion::INTERFACE_VERSION_1:
		return BuildSwapchain1(Swapchain);
		break;
	default:
		assert(false);
		break;
	}

	return false;
}

bool cSwapchainBuilder::BuildSwapchain1(cSwapchain& Swapchain) {
	switch(SurfaceType) {
	case eSurfaceType::SURFACE_TYPE_HWND:
		return BuildSwapchain1ForHWND(Swapchain);
	default:
		break;
	}

	return false;
}

bool cSwapchainBuilder::BuildSwapchain1ForHWND(cSwapchain& Swapchain) {
	if (!hWnd.has_value())
		return false;

	if (!BuildSwapchainDesc1())
		return false;

	if (!BuildSwapchainFullscreenDesc1())
		return false;

	ComPtr<IDXGISwapChain1> SwapchainPtr;
	cCommandQueue* pQueue = Renderer.GetCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT);
	if (pQueue == nullptr)
		return false;

	cCommandQueue& CommandQueue = *pQueue;
	if (!CommandQueue.IsCreated()) {
		assert(false);
		return false;
	}
	auto Result = Renderer.GetHWMngr().GetDXGIFactory()->CreateSwapChainForHwnd(
		CommandQueue.GetCommandQueue(),
		hWnd.value(),
		&Desc1,
		bFullscreen.value() ? &FullScreenDesc : nullptr,
		NULL,
		SwapchainPtr.ReleaseAndGetAddressOf()
	);
	if (Result != S_OK) {
		return false;
	}

	bool bResult = Swapchain.Create(Renderer.GetDevice(), eInterfaceVersion::INTERFACE_VERSION_1, SwapchainPtr.Get());
	if (!bResult) {
		Swapchain.Destroy();
		return false;
	}

	return true;
}

bool cSwapchainBuilder::BuildSwapchainDesc1() {
	if (!Size.has_value())
		return false;
	if (!Format.has_value())
		return false;
	if (!bStereo.has_value())
		return false;
	if (!SampleCount.has_value())
		return false;
	if (!BufferUsage.has_value())
		return false;
	if (!BufferCount.has_value())
		return false;
	if (cSwapchain::MAX_BUFFER_COUNT < BufferCount.value())
		return false;
	if (!Scaling.has_value())
		return false;
	if (!SwapEffect.has_value())
		return false;
	if (!AlphaMode.has_value())
		return false;
	if (!Flags.has_value())
		return false;

	auto SampleDesc = GetDXGISampleDesc(Format.value(), SampleCount.value());
	if (!SampleDesc.has_value())
		return false;

	Desc1.Width = Size->Width;
	Desc1.Height = Size->Height;
	Desc1.Format = Format.value();
	Desc1.Stereo = bStereo.value() ? TRUE : FALSE;
	Desc1.SampleDesc = SampleDesc.value();
	Desc1.BufferUsage = BufferUsage.value();
	Desc1.BufferCount = BufferCount.value();
	Desc1.Scaling = Scaling.value();
	Desc1.SwapEffect = SwapEffect.value();
	Desc1.AlphaMode = AlphaMode.value();
	Desc1.Flags = Flags.value();

	return true;
}

bool cSwapchainBuilder::BuildSwapchainFullscreenDesc1() {
	if (!bFullscreen.has_value())
		return false;
	if (!bFullscreen.value()) {
		return true;
	}

	if (!bWindowed.has_value())
		return false;
	if (!RefreshRateNumerator.has_value())
		return false;
	if (!RefreshRateDenominator.has_value())
		return false;
	if (!ScanlineOrdering.has_value())
		return false;
	if (!FullScreenScaling.has_value())
		return false;

	FullScreenDesc.Windowed					= bWindowed.value();
	FullScreenDesc.RefreshRate.Numerator	= RefreshRateNumerator.value();
	FullScreenDesc.RefreshRate.Denominator	= RefreshRateDenominator.value();
	FullScreenDesc.ScanlineOrdering			= ScanlineOrdering.value();
	FullScreenDesc.Scaling					= FullScreenScaling.value();
	return true;
}

std::optional<DXGI_SAMPLE_DESC> cSwapchainBuilder::GetDXGISampleDesc(DXGI_FORMAT BufferFormat, UINT _SampleCount, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS Flags) const {
	if (!Renderer.GetDevice().IsCreated())
		return std::nullopt;

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS Data;
	Data.SampleCount = _SampleCount;
	Data.Format = BufferFormat;
	Data.Flags = Flags;
	Data.NumQualityLevels = 0;

	if (!Renderer.GetDevice().GetFeatureSupport<D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS>(Data))
		return std::nullopt;

	DXGI_SAMPLE_DESC Desc;
	Desc.Count = _SampleCount;
	Desc.Quality = Data.NumQualityLevels - 1;

	return Desc;
}

cSwapchain::cSwapchain() {
	iCurrentBackBuffer = 0;
}

cSwapchain::~cSwapchain() {
	Destroy();
}

void cSwapchain::Destroy() {
	ReleaseAllBuffers();
	SwapchainPtr.Reset();
}

bool cSwapchain::Resize(cDevice& Device, size_t NewBufferCount, cSizei NewSize, DXGI_FORMAT NewFormat, UINT NewFlags) {
	RETURN_FALSE_IF_NOT_CREATED();

	ReleaseAllBuffers();
	auto Result = SwapchainPtr->ResizeBuffers(static_cast<UINT>(NewBufferCount), NewSize.Width, NewSize.Height, NewFormat, NewFlags);
	if (Result != S_OK) {
		return false;
	}

	BufferCount	= NewBufferCount;
	Size		= NewSize;
	Format		= NewFormat;
	Flags		= NewFlags;

	if (!RetrieveBuffers(Device))
		return false;

	return true;
}

int cSwapchain::Present(UINT SyncInterval, UINT Flags) {
	if (!IsCreated()) {
		return -1;
	}

	DXGI_PRESENT_PARAMETERS Param {0, };
	if (SwapchainPtr->Present1(SyncInterval, Flags, &Param) != S_OK) {
		assert(false);
	}
	int OldBackBufferIndex = static_cast<int>(iCurrentBackBuffer);
	iCurrentBackBuffer = SwapchainPtr->GetCurrentBackBufferIndex();

	return OldBackBufferIndex;
}

const D3D12_CPU_DESCRIPTOR_HANDLE* cSwapchain::GetRenderTargetDescriptor(size_t iDesriptor) const {
	if (!RTVHeap.IsCreated())
		return nullptr;

	return RTVHeap.GetCPUDescriptorHandle(iDesriptor);
}

const D3D12_CPU_DESCRIPTOR_HANDLE* cSwapchain::GetCurrentRenderTargetDescriptor() const {
	return GetRenderTargetDescriptor(GetCurrentBackBufferIndex());
}

bool cSwapchain::BeginFrame(cGraphicsCommandList& CommandList) {
	cResoureBarrier Barrier;
	if (!Barrier.CreateTransition(
		GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		false))
		return false;

	CommandList.GetGraphicsCommandList()->ResourceBarrier(1, Barrier.GetBarrier());
	return true;
}

void cSwapchain::EndFrame(cGraphicsCommandList& CommandList) {
	cResoureBarrier Barrier;
	if (!Barrier.CreateTransition(
		GetCurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT,
		false))
		return;

	CommandList.GetGraphicsCommandList()->ResourceBarrier(1, Barrier.GetBarrier());
	return;
}

bool cSwapchain::Create(cDevice& Device, cSwapchainBuilder::eInterfaceVersion Version, IUnknown* pSwapchain) {
	if (pSwapchain == nullptr)
		return false;

	auto Result = pSwapchain->QueryInterface(IID_PPV_ARGS(SwapchainPtr.ReleaseAndGetAddressOf()));
	if (Result != S_OK)
		return false;

	if (SwapchainPtr.Get() == nullptr)
		return false;

	DXGI_SWAP_CHAIN_DESC1 Desc1;
	if (SwapchainPtr->GetDesc1(&Desc1) != S_OK) {
		assert(false);
		Destroy();
		return false;
	}

	BufferCount = Desc1.BufferCount;
	Format = Desc1.Format;
	Size = cSizei(Desc1.Width, Desc1.Height);
	Flags = Desc1.Flags;

	if (!RetrieveBuffers(Device)) {
		Destroy();
		return false;
	}

	return true;
}

bool cSwapchain::RetrieveBuffers(cDevice& Device) {
	RETURN_FALSE_IF_NOT_CREATED();

	ComPtr<ID3D12Resource> Resource;
	for (size_t iBuffer = 0; iBuffer < BufferCount; ++iBuffer) {
		if (SwapchainPtr->GetBuffer(static_cast<UINT>(iBuffer), IID_PPV_ARGS(Resource.ReleaseAndGetAddressOf())) != S_OK) {
			Destroy();
			return false;
		}

		if (!Buffers[iBuffer].Create(Resource.Get())) {
			Destroy();
			return false;
		}
	}

	if (!RTVHeap.Create(Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, BufferCount, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)) {
		Destroy();
		return false;
	}
	D3D12_RENDER_TARGET_VIEW_DESC RTVDesc;
	RTVDesc.Format = Format;
	RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;
	RTVDesc.Texture2D.PlaneSlice = 0;
	for (size_t iBuffer = 0; iBuffer < BufferCount; ++iBuffer) {
		if (!RTVHeap.CreateRenderTargetView(Device, Buffers[iBuffer], iBuffer, RTVDesc)) {
			Destroy();
			return false;
		}
	}

	iCurrentBackBuffer = SwapchainPtr->GetCurrentBackBufferIndex();

	return true;
}

void cSwapchain::ReleaseAllBuffers() {
	RTVHeap.Destroy();
	for (cResource& Buffer : Buffers) {
		Buffer.Destroy();
	}
}
