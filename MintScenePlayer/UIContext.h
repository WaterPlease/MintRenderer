#pragma once

#include <d3d12.h>

#include "imgui.h"
#include "MintRenderer.h"

class cUIContext {
public:
	bool bShowDemoWindow;
	bool bShowAnotherWindow;

	cUIContext();
	~cUIContext();

	bool Init(HANDLE hWnd, MintChoco::cMintRenderer& Renderer, const MintChoco::cSwapchain& Swapchain);
	void Deinit();

	void PreDraw();
	void Draw(MintChoco::cGraphicsCommandList& CmdList);

protected:
	MintChoco::cDescriptorHeap SrvDescHeapForImGui;
};

