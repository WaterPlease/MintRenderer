#include "pch.h"
#include "Renderer.h"

using namespace MintChoco;

std::mutex cRendererDebugContext::Lock;

cRendererDebugContext& cRendererDebugContext::GetInstance() {
	static cRendererDebugContext Instance;
	return Instance;
}

bool cRendererDebugContext::Initialize() {
	std::lock_guard<std::mutex> LockGuard(Lock);
	cRendererDebugContext& Instance = GetInstance();

	return Instance.InternalInitialize();
}

void cRendererDebugContext::Deinitialize() {
	std::lock_guard<std::mutex> LockGuard(Lock);
	cRendererDebugContext& Instance = GetInstance();

	Instance.InternalDeinitialize();
}

cRendererDebugContext::cRendererDebugContext() {
	bInitalized = false;
}

bool cRendererDebugContext::InternalInitialize() {
	MintChoco::cDXGIDebug DXGIDebug;
	DXGIDebug.Initialize();

	MintChoco::cD3D12Debug Debug;
	Debug.Initialize();

	bInitalized = true;
	return true;
}

void cRendererDebugContext::InternalDeinitialize() {
	D3D12Debug.Deinitialize();

	DXGIDebug.ReportLiveObjects();
	DXGIDebug.Deinitialize();

	bInitalized = false;
	return;
}

cBaseD3D12Renderer::cBaseD3D12Renderer() {

}

cBaseD3D12Renderer::~cBaseD3D12Renderer() {

}

bool cBaseD3D12Renderer::InitHWMngr() {
	if (!HWMngr.Initialize())
		return false;

	BOOL bTearingSupport;
	bool bResult = HWMngr.GetDXGIFactory()->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &bTearingSupport, sizeof(bTearingSupport)) == S_OK;
	if (bResult && (bTearingSupport == TRUE))
		bSupportTearing = true;
	else
		bSupportTearing = false;

	return true;
}

bool cBaseD3D12Renderer::CreateDevice(size_t iGPU, cDevice::eFeatureLevel MinFeatureLevel,
	cDevice::eFeatureLevel MaxFeatureLevel) {

	if (HWMngr.GetGPUCount() <= iGPU) {
		assert(false);
		return false;
	}

	return Device.Create(HWMngr.GetGPUManager(0), MinFeatureLevel, MaxFeatureLevel);
}

bool cBaseD3D12Renderer::InitDeviceDebug() {
	if (!Device.IsCreated()) {
		return false;
	}

	return DeviceDebug.Initialize(Device);
}

void cBaseD3D12Renderer::Destroy(bool bReportLiveDeviceObject) {
	Device.Destroy();
	DeviceDebug.Deinitialize();
	HWMngr.Deinitialize();
	if (bReportLiveDeviceObject) {
		DeviceDebug.ReportLiveDeviceObjects();
	}
}

cCommandQueue* cBaseD3D12Renderer::GetCommandQueue(cDevice::eCommandType CmdType) const {
	return Device.GetCommandQueue(CmdType);
}

bool cBaseD3D12Renderer::WaitCommandQueue(cDevice::eCommandType CmdType, DWORD Timeout) const {
	auto pCommandQueue = GetCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT);
	if (pCommandQueue) {
		return pCommandQueue->Sync(Device, INFINITE);
	}
	return false;
}

bool cBaseD3D12Renderer::FlushCommandQueue(cDevice::eCommandType CmdType, size_t BufferCount, DWORD Timeout) const {
	cFence Fence;
	if (!Fence.Create(Device, 0, D3D12_FENCE_FLAG_NONE)) {
		assert(false);
		return false;
	}

	cEvent Event;
	if (!Event.Create(false, false)) {
		assert(false);
		return false;
	}

	auto pCommandQueue = GetCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT);
	if (!pCommandQueue)
		return false;

	for (size_t iBuffer = 0; iBuffer < BufferCount; ++iBuffer) {
		size_t NextFenceValue = Fence.GetValue().value() + 1;
		if (!pCommandQueue->Signal(Fence, static_cast<UINT>(NextFenceValue))) {
			assert(false);
			return false;
		}

		if (Fence.GetValue() < NextFenceValue) {
			if (!Fence.SetEventOn(NextFenceValue, Event)) {
				assert(false);
				return false;
			}
			if (Event.Wait(Timeout) != WAIT_OBJECT_0) {
				auto CurFence = Fence.GetValue().value();
				return false;
			}
			Event.SetEvent(false);
		}
	}

	return true;
}
