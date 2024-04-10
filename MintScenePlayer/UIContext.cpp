#include "UIContext.h"

#include "App.h"

#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

cUIContext::cUIContext() {
	bShowDemoWindow = true;
	bShowAnotherWindow = false;
}

cUIContext::~cUIContext() {

}

bool cUIContext::Init(HANDLE hWnd, MintChoco::cMintRenderer& Renderer, const MintChoco::cSwapchain& Swapchain) {
	if (!SrvDescHeapForImGui.Create(Renderer.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
		return false;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX12_Init(
		Renderer.GetDevice().GetDevice(), Swapchain.GetBufferCount(), Swapchain.GetFormat(),
		SrvDescHeapForImGui.GetDescriptorHeap(),
		*SrvDescHeapForImGui.GetCPUDescriptorHandle(0), *SrvDescHeapForImGui.GetGPUDescriptorHandle(0));

	return true;
}

void cUIContext::Deinit() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void cUIContext::PreDraw() {
	ImGuiIO& io = ImGui::GetIO();
	// ImGui Rendering => PrepareImGuiRenderData로 옮기기
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (bShowDemoWindow)
			ImGui::ShowDemoWindow(&bShowDemoWindow);
		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &bShowDemoWindow);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &bShowAnotherWindow);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", GetApp().GetClearColor()); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (bShowAnotherWindow)
		{
			ImGui::Begin("Another Window", &bShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				bShowAnotherWindow = false;
			ImGui::End();
		}

		ImGui::Render();
	}
}

void cUIContext::Draw(MintChoco::cGraphicsCommandList& CmdList) {
	CmdList.SetDescriptorHeap(SrvDescHeapForImGui);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CmdList.GetGraphicsCommandList());
}
