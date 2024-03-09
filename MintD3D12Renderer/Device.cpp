#include "pch.h"
#include "Device.h"

#include <vector>

#include "HardwareManager.h"

using namespace MintChoco;

DEFINE_ENUM_NEXT_ADD(MintChoco, cDevice::eFeatureLevel, cDevice::eFeatureLevel::FEATURE_LEVEL_COUNT)

cDevice::cDevice() {

}

cDevice::~cDevice() {
	Destroy();
}

bool cDevice::Create(cGPUManager& GPUManager, eFeatureLevel MinFeatureLevel, eFeatureLevel MaxFeatureLevel) {
	cGPUManager::IDXGIAdapterN* pAdapter = GPUManager.GetAdapter();
	if (!GPUManager.IsInitialized())
		return false;

	if (MaxFeatureLevel == eFeatureLevel::FEATURE_LEVEL_COUNT) {
		MaxFeatureLevel = static_cast<eFeatureLevel>(static_cast<int>(eFeatureLevel::FEATURE_LEVEL_COUNT) - 1);
	}
	if (static_cast<int>(MaxFeatureLevel) - static_cast<int>(MinFeatureLevel) < 0)
		return false;

	ComPtr<ID3D12DeviceN> _DevicePtr;
	auto Result = D3D12CreateDevice(
		pAdapter,
		ConvertToD3DFeatureLevel(MinFeatureLevel),
		IID_PPV_ARGS(_DevicePtr.ReleaseAndGetAddressOf()));
	if (Result != S_OK)
		return false;

	if (_DevicePtr.Get() == nullptr)
		return false;

	size_t NumFeatureLevels = static_cast<int>(MaxFeatureLevel) - static_cast<int>(MinFeatureLevel) + 1;
	std::vector<D3D_FEATURE_LEVEL> Levels(NumFeatureLevels);
	size_t iLevel = 0;
	for (eFeatureLevel Level = MinFeatureLevel; Level <= MaxFeatureLevel; ++Level) {
		Levels[iLevel++] = ConvertToD3DFeatureLevel(Level);
	}

	D3D12_FEATURE_DATA_FEATURE_LEVELS SupportedFeatureLevels;
	SupportedFeatureLevels.NumFeatureLevels = static_cast<UINT>(NumFeatureLevels);
	SupportedFeatureLevels.pFeatureLevelsRequested = Levels.data();

	if (!GetFeatureSupport<D3D12_FEATURE_FEATURE_LEVELS>(_DevicePtr.Get(), SupportedFeatureLevels))
		return false;

	FeatureLevel = ConvertToFeatureLevel(SupportedFeatureLevels.MaxSupportedFeatureLevel);
	if (FeatureLevel == eFeatureLevel::FEATURE_LEVEL_COUNT)
		return false;

	Result = D3D12CreateDevice(
		pAdapter,
		ConvertToD3DFeatureLevel(FeatureLevel),
		IID_PPV_ARGS(_DevicePtr.ReleaseAndGetAddressOf()));
	if (Result != S_OK)
		return false;

	if (_DevicePtr.Get() == nullptr)
		return false;

	DevicePtr = std::move(_DevicePtr);
	return true;
}

void cDevice::Destroy() {
	for (cCommandQueue& CommandQueue : CommandQueues) {
		CommandQueue.Destroy();
	}
	DevicePtr.Reset();
}

cDevice::eFeatureLevel cDevice::GetFeatureLevel() const {
	if (!IsCreated()) 
		return eFeatureLevel::FEATURE_LEVEL_COUNT;

	return FeatureLevel;
}

bool cDevice::IsCommandQueueCreated(eCommandType Type) const {
	return CommandQueues[static_cast<int>(Type)].IsCreated();
}

bool cDevice::CreateCommandQueue(D3D12_COMMAND_LIST_TYPE Type, D3D12_COMMAND_QUEUE_PRIORITY Priority,
	D3D12_COMMAND_QUEUE_FLAGS Flags, UINT NodeMask) {
	if (static_cast<int>(Type) < 0 || cCommandQueue::TypeCount <= static_cast<int>(Type))
		return false;

	return CommandQueues[Type].Create(*this, Type, Priority, Flags, NodeMask);
}

cCommandQueue* cDevice::GetCommandQueue(eCommandType Type) {
	RETURN_NULLPTR_IF_NOT_CREATED();

	if (!CommandQueues[static_cast<int>(Type)].IsCreated()) {
		if (!CommandQueues[static_cast<int>(Type)].Create(*this, ConvertToD3DCommandListType(Type))) {
			return nullptr;
		}
	}
	if (!CommandQueues[static_cast<int>(Type)].IsCreated()) {
		return nullptr;
	}

	return &CommandQueues[static_cast<int>(Type)];
}

cCommandQueue* cDevice::GetCommandQueue(eCommandType Type) const {
	return const_cast<cDevice*>(this)->GetCommandQueue(Type);
}

D3D_FEATURE_LEVEL cDevice::ConvertToD3DFeatureLevel(eFeatureLevel FeatureLevel) {
	switch (FeatureLevel) {
	case eFeatureLevel::FEATURE_LEVEL_11_0: return D3D_FEATURE_LEVEL_11_0;
	case eFeatureLevel::FEATURE_LEVEL_11_1: return D3D_FEATURE_LEVEL_11_1;
	case eFeatureLevel::FEATURE_LEVEL_12_0: return D3D_FEATURE_LEVEL_12_0;
	case eFeatureLevel::FEATURE_LEVEL_12_1: return D3D_FEATURE_LEVEL_12_1;
	case eFeatureLevel::FEATURE_LEVEL_12_2: return D3D_FEATURE_LEVEL_12_2;
	default:
		break;
	}

	return D3D_FEATURE_LEVEL_12_2;
}

cDevice::eFeatureLevel cDevice::ConvertToFeatureLevel(D3D_FEATURE_LEVEL FeatureLevel) {
	switch (FeatureLevel) {
	case D3D_FEATURE_LEVEL_11_0: return eFeatureLevel::FEATURE_LEVEL_11_0;
	case D3D_FEATURE_LEVEL_11_1: return eFeatureLevel::FEATURE_LEVEL_11_1;
	case D3D_FEATURE_LEVEL_12_0: return eFeatureLevel::FEATURE_LEVEL_12_0;
	case D3D_FEATURE_LEVEL_12_1: return eFeatureLevel::FEATURE_LEVEL_12_1;
	case D3D_FEATURE_LEVEL_12_2: return eFeatureLevel::FEATURE_LEVEL_12_2;
	default:
		assert(false);
		break;
	}

	return eFeatureLevel::FEATURE_LEVEL_COUNT;
}

D3D12_COMMAND_LIST_TYPE cDevice::ConvertToD3DCommandListType(eCommandType Type) {
	switch(Type) {
	case eCommandType::COMMAND_TYPE_DIRECT: 
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	case eCommandType::COMMAND_TYPE_BUNDLE: 
		return D3D12_COMMAND_LIST_TYPE_BUNDLE;
	case eCommandType::COMMAND_TYPE_COMPUTE: 
		return D3D12_COMMAND_LIST_TYPE_COMPUTE;
	case eCommandType::COMMAND_TYPE_COPY: 
		return D3D12_COMMAND_LIST_TYPE_COPY;
	case eCommandType::COMMAND_TYPE_VIDEO_DECODE: 
		return D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE;
	case eCommandType::COMMAND_TYPE_VIDEO_PROCESS: 
		return D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS;
	case eCommandType::COMMAND_TYPE_VIDEO_ENCODE: 
		return D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE;

	case eCommandType::COMMAND_TYPE_COUNT:
	default:
		assert(false);
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	}
}

cDevice::eCommandType cDevice::ConvertToCommandType(D3D12_COMMAND_LIST_TYPE Type) {
	switch (Type) {
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		return eCommandType::COMMAND_TYPE_DIRECT;
	case D3D12_COMMAND_LIST_TYPE_BUNDLE:
		return eCommandType::COMMAND_TYPE_BUNDLE;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		return eCommandType::COMMAND_TYPE_COMPUTE;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		return eCommandType::COMMAND_TYPE_COPY;
	case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE:
		return eCommandType::COMMAND_TYPE_VIDEO_DECODE;
	case D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS:
		return eCommandType::COMMAND_TYPE_VIDEO_PROCESS;
	case D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE:
		return eCommandType::COMMAND_TYPE_VIDEO_ENCODE;

	default:
		assert(false);
		return eCommandType::COMMAND_TYPE_DIRECT;
	}
}
