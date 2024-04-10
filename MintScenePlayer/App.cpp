#include "App.h"

#include "ComObject.h"

using namespace MintChoco;

cMintScenePlayerApp::cMintScenePlayerApp() {
	hMainFrame = nullptr;
	bNeedToResizeSwapchain = false;
	bUseVsync = false;
}

cMintScenePlayerApp::~cMintScenePlayerApp() {
	Destroy();
}

bool cMintScenePlayerApp::Create() {
	MintChoco::cComLibs::Init();

	if (IsCreated()) {
		assert(false);
		return false;
	}

	if (!CreateMainFrame())
		return false;

	if (!CreateRenderer())
		return false;

	if (!InitImGUI())
		return false;

	return true;
}

void cMintScenePlayerApp::Destroy() {
	if (!IsCreated())
		return;

	DeinitImGUI();
	DestroyRenderer();
	DestroyMainFrame();
	MintChoco::cComLibs::Deinit();
}

void cMintScenePlayerApp::Run() {
	cCommandQueue* pCommandQueue = Renderer.GetCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT);
	if (pCommandQueue == nullptr)
		return;
	cCommandQueue& CommandQueue = *pCommandQueue;

	if (!FrameSync.Create(Renderer.GetDevice(), Swapchain.GetBufferCount()))
		return;

	bool bQuit = false;
	while(!bQuit) {
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				bQuit = true;
		}
		if (bQuit)
			break;

		if (bNeedToResizeSwapchain) {
			if (!ResizeSwapchain()) {
				assert(false);
				break;
			}
			bNeedToResizeSwapchain = false;
		}

		Draw(CommandQueue);
	}
	Renderer.FlushCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT, Swapchain.GetBufferCount(), 5000);
	FrameSync.Destroy();
}

bool cMintScenePlayerApp::CreateMainFrame() {
	WNDCLASSEXW wc = {
		sizeof(wc), CS_CLASSDC,
		WndProc, 0L, 0L,
		GetModuleHandle(nullptr),
		nullptr, nullptr, nullptr, nullptr,
		L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	MainFrameSize = cSizei(1280, 800);
	hMainFrame = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX12 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

	if (!hMainFrame)
		return false;

	::ShowWindow(hMainFrame, SW_SHOWDEFAULT);
	::UpdateWindow(hMainFrame);

	ClearColor[0] = 0.45f;
	ClearColor[1] = 0.55f;
	ClearColor[2] = 0.60f;
	ClearColor[3] = 1.00f;

	return true;
}

void cMintScenePlayerApp::DestroyMainFrame() {
	if (hMainFrame)
		::DestroyWindow(hMainFrame);
	hMainFrame = nullptr;
}

bool cMintScenePlayerApp::CreateRenderer() {
#ifdef _DEBUG
	cRendererDebugContext& DebugContext = cRendererDebugContext::GetInstance();
	DebugContext.Initialize();
#endif
	if (!Renderer.InitHWMngr())
		return false;

	if (!Renderer.CreateDevice(0))
		return false;
#ifdef _DEBUG
	if (!Renderer.InitDeviceDebug()) {
		return false;
	}
#endif

	cSwapchainBuilder SwapchainBuilder(Renderer);
	cSwapchainBuilder::sSwapchainDesc1Setter Setter(SwapchainBuilder);
	Setter.InterfaceVersion = cSwapchainBuilder::eInterfaceVersion::INTERFACE_VERSION_1;
	Setter.SurfaceType = cSwapchainBuilder::eSurfaceType::SURFACE_TYPE_HWND;
	Setter.hWnd = hMainFrame;
	Setter.Size = cSizei{0, 0};
	Setter.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Setter.bStereo = false;
	Setter.SampleCount = 1;
	Setter.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Setter.BufferCount = BACK_BUFFER_COUNT;
	Setter.Scaling = DXGI_SCALING_STRETCH;
	Setter.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Setter.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	Setter.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	cSwapchainBuilder::sSwapchainFullscreenDesc1Setter FullscreenSetter(SwapchainBuilder);
	FullscreenSetter.bFullscreen = false;

	if (!SwapchainBuilder.Build(Swapchain)) {
		return false;
	}
	bNeedToResizeSwapchain = false;

	return true;
}

void cMintScenePlayerApp::DestroyRenderer() {
	Swapchain.Destroy();
	Renderer.Destroy();
#ifdef _DEBUG
	cRendererDebugContext& DebugContext = cRendererDebugContext::GetInstance();
	DebugContext.Deinitialize();
#endif
}

bool cMintScenePlayerApp::InitImGUI() {
	return UIContext.Init(hMainFrame, Renderer, Swapchain);
}

void cMintScenePlayerApp::DeinitImGUI() {
	UIContext.Deinit();
}

void cMintScenePlayerApp::PrepareImGuiRenderData() {

}

bool cMintScenePlayerApp::ResizeSwapchain() {
	FrameSync.Destroy();

	Renderer.FlushCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT, Swapchain.GetBufferCount(), 5000);
	if (!Swapchain.Resize(Renderer.GetDevice(), Swapchain.GetBufferCount(), MainFrameSize, Swapchain.GetFormat(), Swapchain.GetFlags())) {
		assert(false);
		return false;
	}
	if (!FrameSync.Create(Renderer.GetDevice(), Swapchain.GetBufferCount()))
		return false;

	return true;
}

void cMintScenePlayerApp::Draw(cCommandQueue& CommandQueue) {
	UIContext.PreDraw();

	FrameSync.WaitBackBuffer(Swapchain.GetCurrentBackBufferIndex());

	FrameSync.InitGraphicsCommandList(Swapchain.GetCurrentBackBufferIndex());
	cGraphicsCommandList& CommandList = FrameSync.GetGraphicsCommandList(Swapchain.GetCurrentBackBufferIndex());

	if (!Swapchain.BeginFrame(CommandList))
		return;

	if (!Swapchain.GetCurrentRenderTargetDescriptor()) {
		return;
	}
	
	CommandList.ClearRenderTargetView(*Swapchain.GetCurrentRenderTargetDescriptor(), ClearColor, 0, nullptr);
	CommandList.OMSetRenderTargetsDiscrete(1, Swapchain.GetCurrentRenderTargetDescriptor(), nullptr);

	// Draw UI
	UIContext.Draw(CommandList);

	Swapchain.EndFrame(CommandList);

	CommandList.Close();
	CommandQueue.ExecuteCommandList(CommandList);

	int LastBackBufferIndex = -1;
	if (!bUseVsync) {
		LastBackBufferIndex = Swapchain.Present(0, Renderer.IsSupportTearing() ? DXGI_PRESENT_ALLOW_TEARING : 0);
	}
	else {
		LastBackBufferIndex = Swapchain.Present(1, 0);
	}
	if (LastBackBufferIndex < 0)
		return;
	FrameSync.Signal(CommandQueue, LastBackBufferIndex);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT cMintScenePlayerApp::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (GetApp().Renderer.GetDevice().IsCreated() && wParam != SIZE_MINIMIZED)
		{
			GetApp().MainFrameSize = cSizei((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
			GetApp().bNeedToResizeSwapchain = true;
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

float* cMintScenePlayerApp::GetClearColor() {
	return ClearColor;
}


cMintScenePlayerApp& GetApp() {
	static cMintScenePlayerApp App;
	return App;
}
