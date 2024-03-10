#pragma once

#include <Windows.h>

#include "MintRenderer.h"

class cMintScenePlayerApp {
	friend cMintScenePlayerApp& GetApp();
public:
	enum {
		BACK_BUFFER_COUNT = 3,
	};

	~cMintScenePlayerApp();

	bool Create();
	void Destroy();

	bool IsCreated() const { return hMainFrame; }

	void Run();

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	HWND hMainFrame;

	MintChoco::cSizei	MainFrameSize;
	bool				bNeedToResizeSwapchain;
	bool				bUseVsync;

	MintChoco::cMintRenderer Renderer;
	MintChoco::cSwapchain	 Swapchain;

	MintChoco::cDescriptorHeap SrvDescHeapForImGui;

	cMintScenePlayerApp();

	bool CreateMainFrame();
	void DestroyMainFrame();

	bool CreateRenderer();
	void DestroyRenderer();

	bool InitImGUI();
	void DeinitImGUI();
	void PrepareImGuiRenderData();

	bool ResizeSwapchain();
};

cMintScenePlayerApp& GetApp();