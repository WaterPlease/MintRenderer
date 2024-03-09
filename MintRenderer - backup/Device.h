#pragma once

#include <optional>
#include <array>

#include <wrl.h>
#include <d3d12.h>

#include "Command.h"

#include "Utilities.h"


namespace MintChoco {
#define DefineFeatureDataTypeConversion(_Feature, DataType) \
	template<>\
	struct GetD3D12FeatureDataType<_Feature> {\
		using Type = DataType;\
        static constexpr D3D12_FEATURE Feature = _Feature; \
	};

    template<D3D12_FEATURE _Feature>
    struct GetD3D12FeatureDataType {
        using Type = void;
        static constexpr D3D12_FEATURE Feature = _Feature;
    };

    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS, D3D12_FEATURE_DATA_D3D12_OPTIONS)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_ARCHITECTURE, D3D12_FEATURE_DATA_ARCHITECTURE)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_FEATURE_LEVELS, D3D12_FEATURE_DATA_FEATURE_LEVELS)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_FORMAT_SUPPORT, D3D12_FEATURE_DATA_FORMAT_SUPPORT)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_FORMAT_INFO, D3D12_FEATURE_DATA_FORMAT_INFO)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_SHADER_MODEL, D3D12_FEATURE_DATA_SHADER_MODEL)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS1, D3D12_FEATURE_DATA_D3D12_OPTIONS1)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_PROTECTED_RESOURCE_SESSION_SUPPORT, D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_SUPPORT)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_ROOT_SIGNATURE, D3D12_FEATURE_DATA_ROOT_SIGNATURE)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_ARCHITECTURE1, D3D12_FEATURE_DATA_ARCHITECTURE1)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS2, D3D12_FEATURE_DATA_D3D12_OPTIONS2)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_SHADER_CACHE, D3D12_FEATURE_DATA_SHADER_CACHE)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_COMMAND_QUEUE_PRIORITY, D3D12_FEATURE_DATA_COMMAND_QUEUE_PRIORITY)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS3, D3D12_FEATURE_DATA_D3D12_OPTIONS3)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_EXISTING_HEAPS, D3D12_FEATURE_DATA_EXISTING_HEAPS)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS4, D3D12_FEATURE_DATA_D3D12_OPTIONS4)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_SERIALIZATION, D3D12_FEATURE_DATA_SERIALIZATION)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_CROSS_NODE, D3D12_FEATURE_DATA_CROSS_NODE)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS5, D3D12_FEATURE_DATA_D3D12_OPTIONS5)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_DISPLAYABLE, D3D12_FEATURE_DATA_DISPLAYABLE)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS6, D3D12_FEATURE_DATA_D3D12_OPTIONS6)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_QUERY_META_COMMAND, D3D12_FEATURE_DATA_QUERY_META_COMMAND)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS7, D3D12_FEATURE_DATA_D3D12_OPTIONS7)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_PROTECTED_RESOURCE_SESSION_TYPE_COUNT, D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_TYPE_COUNT)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_PROTECTED_RESOURCE_SESSION_TYPES, D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_TYPES)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS8, D3D12_FEATURE_DATA_D3D12_OPTIONS8)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS9, D3D12_FEATURE_DATA_D3D12_OPTIONS9)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS10, D3D12_FEATURE_DATA_D3D12_OPTIONS10)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS11, D3D12_FEATURE_DATA_D3D12_OPTIONS11)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS12, D3D12_FEATURE_DATA_D3D12_OPTIONS12)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS13, D3D12_FEATURE_DATA_D3D12_OPTIONS13)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS19, D3D12_FEATURE_DATA_D3D12_OPTIONS19)
    DefineFeatureDataTypeConversion(D3D12_FEATURE_D3D12_OPTIONS20, D3D12_FEATURE_DATA_D3D12_OPTIONS20)


#undef DefineFeatureDataTypeConversion
}

namespace MintChoco {
    using Microsoft::WRL::ComPtr;
	class cGPUManager;
	class cDevice {
        DELETE_CLASS_COPY(cDevice);
	public:
		using ID3D12DeviceN = ID3D12Device10;

        enum class eFeatureLevel{
            FEATURE_LEVEL_11_0,
            FEATURE_LEVEL_11_1,
            FEATURE_LEVEL_12_0,
            FEATURE_LEVEL_12_1,
            FEATURE_LEVEL_12_2,

            FEATURE_LEVEL_COUNT
        };

        enum class eCommandType {
            COMMAND_TYPE_DIRECT,
            COMMAND_TYPE_BUNDLE,
            COMMAND_TYPE_COMPUTE,
            COMMAND_TYPE_COPY,
            COMMAND_TYPE_VIDEO_DECODE,
            COMMAND_TYPE_VIDEO_PROCESS,
            COMMAND_TYPE_VIDEO_ENCODE,

            COMMAND_TYPE_COUNT
        };

		cDevice();
		~cDevice();

		bool Create(cGPUManager& GPUManager, eFeatureLevel MinFeatureLevel, eFeatureLevel MaxFeatureLevel);
		void Destroy();

		bool IsCreated() const { return DevicePtr.Get() != nullptr; }
		ID3D12DeviceN* GetDevice() const { return DevicePtr.Get(); }

        eFeatureLevel GetFeatureLevel() const;

		template<D3D12_FEATURE Feature>
        bool GetFeatureSupport(typename GetD3D12FeatureDataType<Feature>::Type& FeatureData) {
            return GetFeatureSupport<Feature>(DevicePtr.Get(), FeatureData);
		}

        bool IsCommandQueueCreated(eCommandType Type) const;
        bool CreateCommandQueue(D3D12_COMMAND_LIST_TYPE Type, D3D12_COMMAND_QUEUE_PRIORITY Priority, D3D12_COMMAND_QUEUE_FLAGS Flags, UINT NodeMask);
        cCommandQueue* GetCommandQueue(eCommandType Type);
        cCommandQueue* GetCommandQueue(eCommandType Type) const;

        static D3D_FEATURE_LEVEL ConvertToD3DFeatureLevel(eFeatureLevel FeatureLevel);
        static eFeatureLevel ConvertToFeatureLevel(D3D_FEATURE_LEVEL FeatureLevel);
        static D3D12_COMMAND_LIST_TYPE ConvertToD3DCommandListType(eCommandType Type);
        static eCommandType ConvertToCommandType(D3D12_COMMAND_LIST_TYPE Type);
	protected:
		ComPtr<ID3D12DeviceN>   DevicePtr;

        eFeatureLevel           FeatureLevel;

        std::array<cCommandQueue, cCommandQueue::TypeCount> CommandQueues;

        template<D3D12_FEATURE Feature,
            typename std::enable_if_t<std::is_void<typename GetD3D12FeatureDataType<Feature>::Type>::value>* = nullptr>
        static bool GetFeatureSupport(ID3D12DeviceN* pDevice, typename GetD3D12FeatureDataType<Feature>::Type& FeatureData) {
            return false;
        }
        template<D3D12_FEATURE Feature,
            typename std::enable_if_t<!std::is_void<typename GetD3D12FeatureDataType<Feature>::Type>::value>* = nullptr>
        static bool GetFeatureSupport(ID3D12DeviceN* pDevice, typename GetD3D12FeatureDataType<Feature>::Type& FeatureData) {
            if (pDevice == nullptr)
                return false;

            auto Result = pDevice->CheckFeatureSupport(Feature, &FeatureData, sizeof(typename GetD3D12FeatureDataType<Feature>::Type));
            if (Result != S_OK)
                return false;

            return true;
        }
	};
    DECLARE_ENUM_NEXT_ADD(cDevice::eFeatureLevel, cDevice::eFeatureLevel::FEATURE_LEVEL_COUNT)
}
