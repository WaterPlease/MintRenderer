#include "App.h"

#include "ComObject.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

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

	cFrameSync FrameSync;
	if (!FrameSync.Create(Renderer.GetDevice(), Swapchain.GetBufferCount()))
		return;

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
			FrameSync.Destroy();
			if (!ResizeSwapchain()) {
				assert(false);
				break;
			}
			if (!FrameSync.Create(Renderer.GetDevice(), Swapchain.GetBufferCount()))
				break;
			bNeedToResizeSwapchain = false;
		}


		ImGuiIO& io = ImGui::GetIO();
		// ImGui Rendering => PrepareImGuiRenderData로 옮기기
		{
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}

			// 3. Show another simple window.
			if (show_another_window)
			{
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}

			ImGui::Render();
		}

		FrameSync.WaitBackBuffer(Swapchain.GetCurrentBackBufferIndex());

		FrameSync.InitGraphicsCommandList(Swapchain.GetCurrentBackBufferIndex());
		cGraphicsCommandList& CommandList = FrameSync.GetGraphicsCommandList(Swapchain.GetCurrentBackBufferIndex());

		if (!Swapchain.BeginFrame(CommandList))
			break;

		if (!Swapchain.GetCurrentRenderTargetDescriptor()) {
			break;
		}
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		CommandList.ClearRenderTargetView(*Swapchain.GetCurrentRenderTargetDescriptor(), clear_color_with_alpha, 0, nullptr);
		CommandList.OMSetRenderTargetsDiscrete(1, Swapchain.GetCurrentRenderTargetDescriptor(), nullptr);
		CommandList.SetDescriptorHeap(SrvDescHeapForImGui);
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CommandList.GetGraphicsCommandList());
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
			break;
		FrameSync.Signal(CommandQueue, LastBackBufferIndex);
	}
	Renderer.FlushCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT, Swapchain.GetBufferCount(), 5000);
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
	Setter.BufferCount = 3;
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

	if (!SrvDescHeapForImGui.Create(Renderer.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
		return false;

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
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hMainFrame);
	ImGui_ImplDX12_Init(
		Renderer.GetDevice().GetDevice(), Swapchain.GetBufferCount(), Swapchain.GetFormat(),
		SrvDescHeapForImGui.GetDescriptorHeap(), 
		*SrvDescHeapForImGui.GetCPUDescriptorHandle(0), *SrvDescHeapForImGui.GetGPUDescriptorHandle(0));

	return true;
}

void cMintScenePlayerApp::DeinitImGUI() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void cMintScenePlayerApp::PrepareImGuiRenderData() {

}

bool cMintScenePlayerApp::ResizeSwapchain() {
	Renderer.FlushCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT, Swapchain.GetBufferCount(), 5000);
	if (!Swapchain.Resize(Renderer.GetDevice(), Swapchain.GetBufferCount(), MainFrameSize, Swapchain.GetFormat(), Swapchain.GetFlags())) {
		assert(false);
		return false;
	}
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



cMintScenePlayerApp& GetApp() {
	static cMintScenePlayerApp App;
	return App;
}