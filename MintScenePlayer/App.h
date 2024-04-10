#pragma once

#include <Windows.h>

#include "MintRenderer.h"
#include "UIContext.h"

class cMintScenePlayerApp {
	friend cMintScenePlayerApp& GetApp();
public:
	static constexpr size_t BACK_BUFFER_COUNT = MintChoco::cMintRenderer::MAX_FRAME_IN_FLIGHT;

	~cMintScenePlayerApp();

	bool Create();
	void Destroy();

	bool IsCreated() const { return hMainFrame; }

	void Run();

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	float* GetClearColor();
protected:
	HWND hMainFrame;

	MintChoco::cSizei	MainFrameSize;
	bool				bNeedToResizeSwapchain;
	bool				bUseVsync;

	MintChoco::cMintRenderer Renderer;
	MintChoco::cSwapchain	 Swapchain;

	MintChoco::cFrameSync FrameSync;

	cUIContext UIContext;

	float ClearColor[4];

	cMintScenePlayerApp();

	bool CreateMainFrame();
	void DestroyMainFrame();

	bool CreateRenderer();
	void DestroyRenderer();

	bool InitImGUI();
	void DeinitImGUI();
	void PrepareImGuiRenderData();

	bool ResizeSwapchain();

	void Draw(MintChoco::cCommandQueue& CommandQueue);
};

cMintScenePlayerApp& GetApp();