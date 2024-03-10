#pragma once

#include "Renderer.h"

namespace MintChoco {

class cMintRenderer;
class cInstantCommandQueue {
	friend cMintRenderer;
public:
	cInstantCommandQueue();
	~cInstantCommandQueue();

	bool IsCreated() const { return pQueue; }
	void Destroy();

	cGraphicsCommandList& GetList() { return CmdList; }

	void Execute();
protected:
	bool Create(const cMintRenderer& Renderer, cDevice::eCommandType Type);
	
	cFence					Fence;
	cCommandQueue*			pQueue;
	cCommandAllocator		Allocator;
	cGraphicsCommandList	CmdList;
};

class cMintRenderer : public cBaseD3D12Renderer {
public:
	cMintRenderer();
	~cMintRenderer();

	bool CreateDevice(size_t iGPU, cDevice::eFeatureLevel MinFeatureLevel = cDevice::eFeatureLevel::FEATURE_LEVEL_11_0, cDevice::eFeatureLevel MaxFeatureLevel = cDevice::eFeatureLevel::FEATURE_LEVEL_COUNT);

	bool CreateInstantCommandQueue(cInstantCommandQueue& Queue, cDevice::eCommandType Type) const;
protected:
	cFence	RendererFence;
	UINT64	RendererFenceValue;
};
}

