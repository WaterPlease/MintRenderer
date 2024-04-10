#pragma once

#include <mutex>

#include "Utilities.h"

#include "DebugLayer.h"
#include "Device.h"
#include "HardwareManager.h"
#include "Synchronize.h"
#include "Swapchain.h"

namespace MintChoco {
	class cRendererDebugContext {
		DELETE_CLASS_COPY(cRendererDebugContext);
	public:
		cDXGIDebug DXGIDebug;
		cD3D12Debug D3D12Debug;

		bool IsInitialized() const { return bInitalized; }
		static cRendererDebugContext& GetInstance();
		static bool Initialize();
		static void Deinitialize();
	protected:
		std::atomic_bool bInitalized;

		static std::mutex Lock;

		cRendererDebugContext();
		bool InternalInitialize();
		void InternalDeinitialize();
	};

	class cBaseD3D12Renderer {
		DELETE_CLASS_COPY(cBaseD3D12Renderer);
	public:
		cBaseD3D12Renderer();
		virtual ~cBaseD3D12Renderer();

		bool InitHWMngr();
		bool CreateDevice(size_t iGPU, cDevice::eFeatureLevel MinFeatureLevel = cDevice::eFeatureLevel::FEATURE_LEVEL_11_0, cDevice::eFeatureLevel MaxFeatureLevel = cDevice::eFeatureLevel::FEATURE_LEVEL_COUNT);
		bool InitDeviceDebug();
		void Destroy(bool bReportLiveDeviceObject = false);

		cHardwareManager&		GetHWMngr()			{ return HWMngr; }
		const cHardwareManager& GetHWMngr() const	{ return HWMngr; }

		cDevice&		GetDevice()			{ return Device; }
		const cDevice&	GetDevice() const	{ return Device; }

		cD3D12DeviceDebug&			GetDeviceDebug()		{ return DeviceDebug; }
		const cD3D12DeviceDebug&	GetDeviceDebug() const	{ return DeviceDebug; }

		cCommandQueue*	GetCommandQueue(cDevice::eCommandType CmdType) const;
		bool			WaitCommandQueue(cDevice::eCommandType CmdType, DWORD Timeout = INFINITE) const;
		bool			FlushCommandQueue(cDevice::eCommandType CmdType, size_t BufferCount, DWORD Timeout = INFINITE) const;

		bool			IsSupportTearing() const { return bSupportTearing; }
	protected:
		cHardwareManager	HWMngr;
		cDevice				Device;
		cD3D12DeviceDebug	DeviceDebug;

		bool				bSupportTearing;
	};
}

