#pragma once

#include <optional>
#include <array>

#include <wrl.h>
#include <Windows.h>

#include "Geometry.h"
#include "Resource.h"
#include "Utilities.h"
#include "Descriptor.h"

namespace MintChoco {
	class cGraphicsCommandList;
	using Microsoft::WRL::ComPtr;
	class cBaseD3D12Renderer;
	class cSwapchain;

	class cSwapchainBuilder {
	public:
		enum class eInterfaceVersion {
			INTERFACE_VERSION_1,

			INTERFACE_VERSION_COUNT
		};

		enum class eSurfaceType {
			SURFACE_TYPE_HWND,

			SURFACE_TYPE_COUNT
		};
		struct sSwapchainDesc1Setter_
		{
			struct ASetter
			{
				ASetter(eInterfaceVersion& Version) : Version(Version) {}

				eInterfaceVersion& Version;
			};
		};

		struct sSwapchainDesc1Setter {
			sSwapchainDesc1Setter(cSwapchainBuilder& Builder)
				:	InterfaceVersion(Builder.InterfaceVersion), SurfaceType(Builder.SurfaceType),
					hWnd(Builder.hWnd), Size(Builder.Size), Format(Builder.Format),
					bStereo(Builder.bStereo), SampleCount(Builder.SampleCount),
					BufferUsage(Builder.BufferUsage), BufferCount(Builder.BufferCount),
					Scaling(Builder.Scaling), SwapEffect(Builder.SwapEffect),
					AlphaMode(Builder.AlphaMode), Flags(Builder.Flags) {
				
			}

			eInterfaceVersion&					InterfaceVersion;
			eSurfaceType&						SurfaceType;
			std::optional<HWND>&				hWnd;
			std::optional<cSizei>&				Size;
			std::optional<DXGI_FORMAT>&			Format;
			std::optional<bool>&				bStereo;
			std::optional<UINT>&				SampleCount;
			std::optional<DXGI_USAGE>&			BufferUsage;
			std::optional<UINT>&				BufferCount;
			std::optional<DXGI_SCALING>&		Scaling;
			std::optional<DXGI_SWAP_EFFECT>&	SwapEffect;
			std::optional<DXGI_ALPHA_MODE>&		AlphaMode;
			std::optional<UINT>&				Flags;
		};

		struct sSwapchainFullscreenDesc1Setter {
			sSwapchainFullscreenDesc1Setter(cSwapchainBuilder& Builder)
				: bFullscreen(Builder.bFullscreen), bWindowed(Builder.bWindowed),
				RefreshRateNumerator(Builder.RefreshRateNumerator), RefreshRateDenominator(Builder.RefreshRateDenominator),
				ScanlineOrdering(Builder.ScanlineOrdering),
				FullScreenScaling(Builder.FullScreenScaling)
			{}

			std::optional<bool>&						bFullscreen;
			std::optional<bool>&						bWindowed;
			std::optional<UINT>&						RefreshRateNumerator;
			std::optional<UINT>&						RefreshRateDenominator;
			std::optional<DXGI_MODE_SCANLINE_ORDER>&	ScanlineOrdering;
			std::optional<DXGI_MODE_SCALING>&			FullScreenScaling;
		};

		eInterfaceVersion	InterfaceVersion;
		eSurfaceType		SurfaceType;

		// Surface
		std::optional<HWND>	hWnd;

		// Swapchain Desc
		std::optional<cSizei> Size;
		std::optional<DXGI_FORMAT> Format;
		std::optional<bool> bStereo;
		std::optional<UINT> SampleCount;
		std::optional<DXGI_USAGE> BufferUsage;
		std::optional<UINT> BufferCount;
		std::optional<DXGI_SCALING> Scaling;
		std::optional<DXGI_SWAP_EFFECT> SwapEffect;
		std::optional<DXGI_ALPHA_MODE> AlphaMode;
		std::optional<UINT> Flags;

		// Swapchain Fullscreen Desc
		std::optional<bool>						bFullscreen;
		std::optional<bool>						bWindowed;
		std::optional<UINT>						RefreshRateNumerator;
		std::optional<UINT>						RefreshRateDenominator;
		std::optional<DXGI_MODE_SCANLINE_ORDER> ScanlineOrdering;
		std::optional<DXGI_MODE_SCALING>        FullScreenScaling;

		cSwapchainBuilder(cBaseD3D12Renderer& Renderer);
		~cSwapchainBuilder();

		void Clear();

		bool Build(cSwapchain& Swapchain);

		void SetDoubleBuffering() { BufferCount = 2; }
		void SetTrippleBuffering() { BufferCount = 3; }
	protected:
		cBaseD3D12Renderer& Renderer;

		DXGI_SWAP_CHAIN_DESC1 Desc1;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC FullScreenDesc;

		bool BuildSwapchain1(cSwapchain& Swapchain);
		bool BuildSwapchain1ForHWND(cSwapchain& Swapchain);

		bool BuildSwapchainDesc1();
		bool BuildSwapchainFullscreenDesc1();

		std::optional<DXGI_SAMPLE_DESC> GetDXGISampleDesc(DXGI_FORMAT BufferFormat, UINT SampleCount, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE) const;
	};

	class cSwapchain {
		friend cSwapchainBuilder;
		DELETE_CLASS_COPY(cSwapchain);
	public:
		using IDXGISwapChainN = IDXGISwapChain4;
		static constexpr cSwapchainBuilder::eInterfaceVersion InterfaceVersion = cSwapchainBuilder::eInterfaceVersion::INTERFACE_VERSION_1;
		static constexpr size_t MAX_BUFFER_COUNT = 4;
		cSwapchain();
		~cSwapchain();

		void Destroy();

		bool IsCreated() const { return SwapchainPtr.Get() != nullptr; }
		IDXGISwapChainN* GetSwapchain() const { return SwapchainPtr.Get(); }

		size_t		GetBufferCount()	const { return BufferCount;}
		DXGI_FORMAT	GetFormat()			const { return Format;}
		cSizei		GetSize()			const { return Size;}
		UINT		GetFlags()			const { return Flags;}

		bool		Resize(cDevice& Device, size_t NewBufferCount, cSizei NewSize, DXGI_FORMAT NewFormat, UINT NewFlags);
		int			Present(UINT SyncInterval, UINT Flags);

		size_t		GetCurrentBackBufferIndex() const { return iCurrentBackBuffer; }
		cResource&	GetCurrentBackBuffer() { return Buffers[iCurrentBackBuffer]; }
		cResource&	GetCurrentBackBuffer() const { return const_cast<cSwapchain*>(this)->GetCurrentBackBuffer(); }

		const D3D12_CPU_DESCRIPTOR_HANDLE* GetRenderTargetDescriptor(size_t iDesriptor) const;
		const D3D12_CPU_DESCRIPTOR_HANDLE* GetCurrentRenderTargetDescriptor() const;

		bool BeginFrame(cGraphicsCommandList& CommandList);
		void EndFrame(cGraphicsCommandList& CommandList);
	protected:
		ComPtr<IDXGISwapChainN> SwapchainPtr;

		size_t		BufferCount;
		DXGI_FORMAT	Format;
		cSizei		Size;
		UINT		Flags;

		size_t		iCurrentBackBuffer;

		std::array<cResource, MAX_BUFFER_COUNT> Buffers;
		cDescriptorHeap							RTVHeap;

		bool Create(cDevice& Device, cSwapchainBuilder::eInterfaceVersion Version, IUnknown* pSwapchain);

		bool RetrieveBuffers(cDevice& Device);
		void ReleaseAllBuffers();
	};
}

namespace MintChoco {
#define DefineGetSwapchainInterface(_Version, Interface)\
	template<>\
	struct GetSwapchainInterface<_Version> {\
		using Type = Interface;\
		static constexpr cSwapchainBuilder::eInterfaceVersion Version = _Version;\
	}

	template<cSwapchainBuilder::eInterfaceVersion _Version>
	struct GetSwapchainInterface {
		using Type = void;
		static constexpr cSwapchainBuilder::eInterfaceVersion Version = _Version;
	};

	DefineGetSwapchainInterface(cSwapchainBuilder::eInterfaceVersion::INTERFACE_VERSION_1, IDXGISwapChain1);

#undef DefineGetSwapchainInterface
}