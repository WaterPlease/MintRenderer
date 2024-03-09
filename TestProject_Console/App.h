#pragma once

#include <iostream>
#include <Windows.h>

#include "Geometry.h"
#include "Pipeline.h"
#include "Renderer.h"
#include "Shader.h"
#include "Image.h"

class cBaseApp {
public:
	cBaseApp();
	cBaseApp(SIZE DefaultSize);
	virtual ~cBaseApp();

	virtual bool Create(HINSTANCE hInstance, LPCTSTR AppName, LPCTSTR Title, int nCmdShow);
	virtual void Destroy();

	virtual void Run() = 0;
	void PollMessage();

	void Exit() { bExitRequested = true; }
	bool IsExiting() const { return bExitRequested; }

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	WNDPROC WndProc;

	HWND	hMainFrame;
	SIZE	Size;

	bool	bExitRequested;
};
extern cBaseApp* pApp;
void		SetApp(cBaseApp* pApp);
cBaseApp*	GetApp();

class cApp : public cBaseApp {
public:
	cApp();
	~cApp();

	virtual bool Create(HINSTANCE hInstance, LPCTSTR AppName, LPCTSTR Title, int nCmdShow) override;
	virtual void Destroy() override;

	virtual void Run() override;

	void	SetFullscreen(bool bFullscreen);
	bool	IsFullscreen() const { return bFullscreen; }

	void	Log(LPCTSTR Msg) const;

	void	OnSize(UINT nType, UINT Width, UINT Height);
protected:
	MintChoco::cBaseD3D12Renderer Renderer;
	MintChoco::cSwapchain Swapchain;

	bool bUseVsync;
	size_t MaxFPS;

	bool bFullscreen;

	bool bUseStdioForLog;
	bool bNeedResizeSwapchain;

	bool ResizeSwapchain();

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	std::wstring ProcessDiectory;

	// Triangle Model
	MintChoco::cShader	 Shader[3];
	MintChoco::cResource RectVertexBuffer;
	MintChoco::cResource RectIndexBuffer;
	D3D12_VERTEX_BUFFER_VIEW RectVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW  RectIndexBufferView;
	MintChoco::cRootSignature RectRootSignature;
	MintChoco::cGraphicsPipelineStateObject RectPipelineStateObject;
	MintChoco::cImage		RectTextureImage;
	MintChoco::cResource	RectTexture;
	MintChoco::cDescriptorHeap RectDescripotrHeap;

	void CreateTriangle();
	void LoadTriangleShader();
	void DestroyTriangle();
	void DrawTriangle(MintChoco::cGraphicsCommandList& CmdList);
};