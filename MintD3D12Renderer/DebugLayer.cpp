#include "pch.h"
#include "DebugLayer.h"
#include "Device.h"

using namespace MintChoco;

cDXGIDebug::cDXGIDebug() {

}

cDXGIDebug::~cDXGIDebug() {
	Deinitialize();
}

bool cDXGIDebug::Initialize() {
	if (DXGIGetDebugInterface1(0, IID_PPV_ARGS(DebugPtr.ReleaseAndGetAddressOf())) != S_OK) {
		return false;
	}

	if (DebugPtr.Get() == nullptr)
		return false;

	return true;
}

void cDXGIDebug::Deinitialize() {
	DebugPtr.Reset();
}

bool cDXGIDebug::ReportLiveObjects() const {
	if (!IsInitialized())
		return false;

	auto Result = DebugPtr->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_DETAIL));
	if (Result != S_OK)
		return false;

	return true;
}

cD3D12Debug::cD3D12Debug() {

}

cD3D12Debug::~cD3D12Debug() {
	Deinitialize();
}

bool cD3D12Debug::Initialize() {
	if (D3D12GetDebugInterface(IID_PPV_ARGS(DebugPtr.ReleaseAndGetAddressOf())) != S_OK) {
		return false;
	}

	if (DebugPtr.Get() == nullptr)
		return false;

	DebugPtr->EnableDebugLayer();
	DebugPtr->SetEnableGPUBasedValidation(TRUE);
	DebugPtr->SetGPUBasedValidationFlags(D3D12_GPU_BASED_VALIDATION_FLAGS_NONE);

	return true;
}

void cD3D12Debug::Deinitialize() {
	DebugPtr.Reset();
}

MintChoco::cD3D12DeviceDebug::cD3D12DeviceDebug() {

}

MintChoco::cD3D12DeviceDebug::~cD3D12DeviceDebug() {

}

bool MintChoco::cD3D12DeviceDebug::Initialize(cDevice& Device) {
	if (!Device.IsCreated())
		return false;

	ComPtr<ID3D12DebugDeviceN> _DebugPtr;
	auto Result = Device.GetDevice()->QueryInterface(IID_PPV_ARGS(_DebugPtr.ReleaseAndGetAddressOf()));
	if (Result != S_OK)
		return false;

	if (_DebugPtr.Get() == nullptr)
		return false;

	ComPtr<ID3D12InfoQueue1> _InfoQueuePtr;
	Device.GetDevice()->QueryInterface(IID_PPV_ARGS(_InfoQueuePtr.ReleaseAndGetAddressOf()));
	if (Result != S_OK)
		return false;

	if (_InfoQueuePtr.Get() == nullptr)
		return false;

	DebugPtr = std::move(_DebugPtr);
	InfoQueuePtr = std::move(_InfoQueuePtr);

	return true;
}

void MintChoco::cD3D12DeviceDebug::Deinitialize() {
	InfoQueuePtr.Reset();
	DebugPtr.Reset();
}

std::optional<D3D12_DEBUG_FEATURE> cD3D12DeviceDebug::GetFeatureMask() const {
	if (!IsInitialized())
		return std::nullopt;

	return DebugPtr->GetFeatureMask();
}

bool cD3D12DeviceDebug::SetFeatureMask(D3D12_DEBUG_FEATURE FeatureMask) {
	if (!IsInitialized())
		return false;

	auto Result = DebugPtr->SetFeatureMask(FeatureMask);
	if (Result != S_OK)
		return false;

	return true;
}

bool cD3D12DeviceDebug::ReportLiveDeviceObjects(D3D12_RLDO_FLAGS Flags) const {
	if (!IsInitialized())
		return false;

	auto Result = DebugPtr->ReportLiveDeviceObjects(Flags);
	if (Result != S_OK)
		return false;

	return true;
}
