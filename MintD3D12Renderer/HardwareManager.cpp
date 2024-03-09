#include "pch.h"
#include "HardwareManager.h"

#include "Utilities.h"

using namespace MintChoco;

cCPUManager::cCPUManager() {

}

cCPUManager::~cCPUManager() {
	Deinitialize();
}

bool cCPUManager::Initialize() {
	return true;
}

void cCPUManager::Deinitialize() {

}

cOutputManager::cOutputManager() {

}

cOutputManager::~cOutputManager() {
	Deinitialize();
}

bool cOutputManager::Initialize(IDXGIOutput6* pOutput) {
	if (pOutput == nullptr)
		return false;

	OutputPtr = pOutput;
	if (OutputPtr->GetDesc1(&Desc) != S_OK) {
		assert(false);
		return false;
	}

	return true;
}

void cOutputManager::Deinitialize() {
	OutputPtr.Reset();
	memset(&Desc, 0, sizeof(Desc));
}

cGPUManager::cGPUManager() {
	OutputCount = 0;
}

cGPUManager::~cGPUManager() {
	Deinitialize();
}

bool cGPUManager::Initialize(IDXGIAdapter4* pAdapter) {
	if (pAdapter == nullptr)
		return false;

	AdapterPtr = pAdapter;
	if (AdapterPtr->GetDesc3(&Desc) != S_OK) {
		assert(false);
		return false;
	}

	if (Desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
		return false;
	}

	OutputCount = 0;
	ComPtr<IDXGIOutput> OutputPtr;
	size_t iOutput = 0;
	while (AdapterPtr->EnumOutputs(static_cast<UINT>(iOutput++), OutputPtr.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		ComPtr<cOutputManager::IDXGIOutputN> Output6Ptr;
		auto Result = OutputPtr->QueryInterface(IID_PPV_ARGS(Output6Ptr.GetAddressOf()));
		if (Result != S_OK)
			continue;

		if (!OutputManagers[OutputCount].Initialize(Output6Ptr.Get())) {
			OutputManagers[OutputCount].Deinitialize();
			continue;
		}

		if (++OutputCount >= MAX_OUPUT_PER_GPU_COUNT)
			break;
	}

	return true;
}

void cGPUManager::Deinitialize() {
	OutputCount = 0;
	for (cOutputManager& OutputManager : OutputManagers) {
		OutputManager.Deinitialize();
	}

	AdapterPtr.Reset();
	memset(&Desc, 0, sizeof(Desc));
}

cHardwareManager::cHardwareManager() {
	CPUCount = 0;
	GPUCount = 0;
	bIsInitialized = false;
}

cHardwareManager::~cHardwareManager() {
	Deinitialize();
}

bool cHardwareManager::Initialize() {
	// Pass Initializing CPU Manager for now.
	// Use WMI for cpu description
	CPUCount = 1;

	UINT FactoryFlags = IF_DEBUG(DXGI_CREATE_FACTORY_DEBUG, 0);
	CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(DXGIFactoryPtr.ReleaseAndGetAddressOf()));

	if (DXGIFactoryPtr.Get() == nullptr)
		return false;

	GPUCount = 0;
	size_t iGPU = 0;
	ComPtr<cGPUManager::IDXGIAdapterN> _AdapterPtr;
	while (DXGIFactoryPtr->EnumAdapterByGpuPreference(static_cast<UINT>(iGPU++), DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(_AdapterPtr.ReleaseAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND) {
		ComPtr<cGPUManager::IDXGIAdapterN> AdapterNPtr;

		auto Result = _AdapterPtr->QueryInterface(IID_PPV_ARGS(AdapterNPtr.GetAddressOf()));
		if (Result != S_OK)
			continue;

		if (!GPUManagers[GPUCount].Initialize(AdapterNPtr.Get())) {
			GPUManagers[GPUCount].Deinitialize();
			continue;
		}

		if (++GPUCount >= MAX_GPU_COUNT)
			break;
	}
	
	bIsInitialized = true;
	return true;
}

void cHardwareManager::Deinitialize() {
	CPUCount = GPUCount = 0;

	for (cCPUManager& CPUManager : CPUManagers) {
		CPUManager.Deinitialize();
	}

	for (cGPUManager& GPUManager : GPUManagers) {
		GPUManager.Deinitialize();
	}

	DXGIFactoryPtr.Reset();
	bIsInitialized = false;
}

cCPUManager& cHardwareManager::GetCPUManager(size_t iCPU) {
	assert(iCPU < MAX_CPU_COUNT);  return CPUManagers[iCPU];
}

cGPUManager& cHardwareManager::GetGPUManager(size_t iGPU) {
	assert(iGPU < MAX_GPU_COUNT);  return GPUManagers[iGPU];
}
