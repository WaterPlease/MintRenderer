#pragma once

#include <iostream>
#include <Windows.h>

#include "Geometry.h"
#include "Pipeline.h"
#include "MintRenderer.h"
#include "Shader.h"
#include "Image.h"
#include "Mesh.h"
#include "Texture.h"

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
	MintChoco::cMintRenderer Renderer;
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

	MintChoco::cPrimitive	RectPrimitive;
	MintChoco::cTexture		RectTexture;
#pragma pack(push, 4)
	struct sTransform {
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 InverseView;
		glm::mat4 Projection;
	};
#pragma pack(pop)
	MintChoco::cResource		RectTransform;
	BYTE*						RectTransformBuffer;
	MintChoco::cDescriptorHeap	RectDescripotrHeap[3];
	glm::vec2					RectPos;
	glm::vec2					MousePos;

	// Triangle Model
	MintChoco::cShader						Shader[3];
	MintChoco::cRootSignature				RectRootSignature;
	MintChoco::cGraphicsPipelineStateObject RectPipelineStateObject;

	void CreateRect();
	void LoadRectShader();
	void DestroyRect();
	void DrawRect(MintChoco::cGraphicsCommandList& CmdList);
};