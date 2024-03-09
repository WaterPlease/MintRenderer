#pragma once

#include <array>

#include <wrl.h>
#include <dxgi1_6.h>

#include "Utilities.h"


namespace MintChoco {
	using Microsoft::WRL::ComPtr;

	constexpr size_t MAX_CPU_COUNT = 1;
	constexpr size_t MAX_GPU_COUNT = 8;
	constexpr size_t MAX_OUPUT_PER_GPU_COUNT = 4;

	class cCPUManager {
		DELETE_CLASS_COPY(cCPUManager)
	public:
		cCPUManager();
		~cCPUManager();

		bool Initialize();
		void Deinitialize();
	protected:
	};

	class cOutputManager {
		DELETE_CLASS_COPY(cOutputManager)
	public:
		using IDXGIOutputN = IDXGIOutput6;

		cOutputManager();
		~cOutputManager();

		bool Initialize(IDXGIOutputN* pOutput);
		void Deinitialize();

		IDXGIOutputN* GetOutput() const { return OutputPtr.Get(); }
	protected:
		ComPtr<IDXGIOutputN>	OutputPtr;
		DXGI_OUTPUT_DESC1		Desc;
	};

	class cGPUManager {
		DELETE_CLASS_COPY(cGPUManager)
	public:
		using IDXGIAdapterN = IDXGIAdapter4;

		cGPUManager();
		~cGPUManager();

		bool Initialize(IDXGIAdapterN* pAdapter);
		void Deinitialize();

		bool IsInitialized() const { return AdapterPtr.Get() != nullptr; }
		IDXGIAdapterN* GetAdapter() const { return AdapterPtr.Get(); }
	protected:
		ComPtr<IDXGIAdapterN>	AdapterPtr;
		DXGI_ADAPTER_DESC3		Desc;

		size_t OutputCount;
		std::array<cOutputManager, MAX_OUPUT_PER_GPU_COUNT> OutputManagers;
	};

	class cHardwareManager {
		DELETE_CLASS_COPY(cHardwareManager)
	public:
		using IDXGIFactoryN = IDXGIFactory7;

		cHardwareManager();
		~cHardwareManager();

		bool Initialize();
		void Deinitialize();

		bool IsInitialized() const { return bIsInitialized; }

		IDXGIFactoryN* GetDXGIFactory() const { return DXGIFactoryPtr.Get(); }

		size_t		 GetCPUCount() const { return CPUCount; }
		cCPUManager& GetCPUManager(size_t iCPU);
		size_t		 GetGPUCount() const { return GPUCount; }
		cGPUManager& GetGPUManager(size_t iGPU);

	protected:
		bool bIsInitialized;
		ComPtr<IDXGIFactoryN> DXGIFactoryPtr;

		size_t CPUCount;
		std::array<cCPUManager, MAX_CPU_COUNT>	CPUManagers;

		size_t GPUCount;
		std::array<cGPUManager, MAX_GPU_COUNT>	GPUManagers;
	};

};

