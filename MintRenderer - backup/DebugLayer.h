#pragma once

#include <wrl.h>
#include <dxgidebug.h>
#include <d3d12sdklayers.h>
#include <optional>

#include "Utilities.h"


namespace MintChoco {
	using Microsoft::WRL::ComPtr;
	class cDXGIDebug {
		DELETE_CLASS_COPY(cDXGIDebug);
	public:
		using IDXGIDebugN = IDXGIDebug1;

		cDXGIDebug();
		~cDXGIDebug();

		bool Initialize();
		void Deinitialize();

		bool IsInitialized() const { return DebugPtr.Get() != nullptr; }
		IDXGIDebugN* GetDebug() const { return DebugPtr.Get(); }

		bool ReportLiveObjects() const;
	protected:
		ComPtr<IDXGIDebugN> DebugPtr;
	};

	class cD3D12Debug {
		DELETE_CLASS_COPY(cD3D12Debug);
	public:
		using ID3D12DebugN = ID3D12Debug6;

		cD3D12Debug();
		~cD3D12Debug();

		bool Initialize();
		void Deinitialize();

		bool IsInitialized() const { return DebugPtr.Get() != nullptr; }
		ID3D12DebugN* GetDebug() const { return DebugPtr.Get(); }

	protected:
		ComPtr<ID3D12DebugN> DebugPtr;
	};

	class cDevice;
	class cD3D12DeviceDebug {
		DELETE_CLASS_COPY(cD3D12DeviceDebug);
	public:
		using ID3D12DebugDeviceN = ID3D12DebugDevice2;

		cD3D12DeviceDebug();
		~cD3D12DeviceDebug();

		bool Initialize(cDevice& Device);
		void Deinitialize();

		bool IsInitialized() const { return DebugPtr.Get() != nullptr; }
		ID3D12DebugDeviceN* GetDebug() const { return DebugPtr.Get(); }

		std::optional<D3D12_DEBUG_FEATURE> GetFeatureMask() const;
		bool SetFeatureMask(D3D12_DEBUG_FEATURE FeatureMask);

		bool ReportLiveDeviceObjects(D3D12_RLDO_FLAGS Flags = (D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL)) const;
	protected:
		ComPtr<ID3D12DebugDeviceN>	DebugPtr;
		ComPtr<ID3D12InfoQueue1>	InfoQueuePtr;
	};
};

