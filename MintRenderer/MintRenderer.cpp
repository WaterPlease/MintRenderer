#include "pch.h"
#include "MintRenderer.h"

using namespace MintChoco;

cInstantCommandQueue::cInstantCommandQueue() {
	pQueue = nullptr;
}

cInstantCommandQueue::~cInstantCommandQueue() {
	Destroy();
}

void cInstantCommandQueue::Execute() {
	if (!IsCreated())
		return;
	if (!CmdList.IsClosed())
		CmdList.Close();

	pQueue->ExecuteCommandList(CmdList);

	cEvent Event;
	if (!Event.Create(false, false))
		assert(false);

	pQueue->Signal(Fence, 1);

	Fence.SetEventOn(1, Event);

	if (Fence.GetValue() < 1) {
		Event.Wait(INFINITE);
	}

	Allocator.Reset();
	CmdList.Reset(Allocator);

	return;
}

bool cInstantCommandQueue::Create(const cMintRenderer& Renderer, cDevice::eCommandType Type) {
	if (!Fence.Create(Renderer.GetDevice(), 0, D3D12_FENCE_FLAG_NONE))
		return false;
	
	pQueue = Renderer.GetCommandQueue(Type);
	if (!pQueue)
		return false;

	D3D12_COMMAND_LIST_TYPE D3D12Type = cDevice::ConvertToD3DCommandListType(Type);
	if (!Allocator.Create(Renderer.GetDevice(), D3D12Type))
		return false;

	if (!CmdList.Create(Renderer.GetDevice(), Allocator, D3D12Type))
		return false;
	CmdList.Reset(Allocator);

	return true;
}

void cInstantCommandQueue::Destroy() {
	Execute();
	pQueue = nullptr;
	CmdList.Destroy();
	Allocator.Destroy();
	Fence.Destroy();
	return;
}

cMintRenderer::cMintRenderer() {
	RendererFenceValue = 0;
}

cMintRenderer::~cMintRenderer() {

}

bool cMintRenderer::CreateDevice(size_t iGPU, cDevice::eFeatureLevel MinFeatureLevel, cDevice::eFeatureLevel MaxFeatureLevel)
{
	if (!cBaseD3D12Renderer::CreateDevice(iGPU, MinFeatureLevel, MaxFeatureLevel))
		return false;

	if (!RendererFence.Create(Device, 0, D3D12_FENCE_FLAG_NONE)) {
		RendererFenceValue = 0;
		return false;
	}

	return true;
}

bool cMintRenderer::CreateInstantCommandQueue(cInstantCommandQueue& Queue, cDevice::eCommandType Type) const {
	if (Queue.IsCreated())
		return false;
		
	return Queue.Create(*this, Type);
}
