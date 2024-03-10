#include "App.h"

#include <cassert>
#include <DirectXMath.h>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace MintChoco;
using namespace DirectX;

cBaseApp::cBaseApp()
	: cBaseApp({ 800, 600 }) {
}

cBaseApp::cBaseApp(SIZE DefaultSize)
	: Size(DefaultSize), bExitRequested(false) {
    WndProc = WindowProc;
}

cBaseApp::~cBaseApp() {
    Destroy();
}

bool cBaseApp::Create(HINSTANCE hInstance, LPCTSTR AppName, LPCTSTR Title, int nCmdShow) {
    WNDCLASSEXW WC = { };
    WC.cbSize = sizeof(WNDCLASSEXW);
    WC.style = CS_OWNDC;
    WC.lpfnWndProc = WndProc;
    WC.cbClsExtra = 0;
    WC.cbWndExtra = 0;
    WC.hInstance = hInstance;
    WC.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    WC.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    WC.hbrBackground = nullptr;
    WC.lpszMenuName = nullptr;
    WC.lpszClassName = AppName;
    WC.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
    RegisterClassExW(&WC);

    hMainFrame = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
        AppName,
        Title,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        Size.cx, Size.cy,
        NULL,        
        NULL,       
        hInstance,  
        NULL        
    );
    if (hMainFrame == NULL)
    {
        return false;
    }

    ShowWindow(hMainFrame, nCmdShow);
    return true;
}

void cBaseApp::Destroy() {

}

void cBaseApp::PollMessage() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
        if (msg.message == WM_QUIT) {
            bExitRequested = true;
            break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT cBaseApp::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
cBaseApp* pApp = nullptr;

void SetApp(cBaseApp* _pApp) {
	assert(pApp == nullptr);
    pApp = _pApp;
}

cBaseApp* GetApp() {
    return pApp;
}

cApp::cApp() : cBaseApp() {
    bUseStdioForLog = false;
    bNeedResizeSwapchain = false;
    bUseVsync = false;
    MaxFPS = 1000;
    bFullscreen = false;
}

cApp::~cApp() {
    Destroy();
}

void GetParentDirectoryOfFile(std::wstring& FilePath) {
    for (int iChar = FilePath.length() - 1; iChar >= 0; --iChar) {
        if (FilePath[iChar] == L'\\') {
            FilePath.resize(iChar);
            return;
        }
    }
}

bool cApp::Create(HINSTANCE hInstance, LPCTSTR AppName, LPCTSTR Title, int nCmdShow) {
	WndProc = WindowProc;
    if (!cBaseApp::Create(hInstance, AppName, Title, nCmdShow)) {
        return false;
    }

    WCHAR _DirectoryPath[1024];
    GetModuleFileNameW(nullptr, _DirectoryPath, 1024 * sizeof(WCHAR));

    ProcessDiectory = _DirectoryPath;
    GetParentDirectoryOfFile(ProcessDiectory);
    ProcessDiectory += L'\\';

    if (bUseStdioForLog) {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "wt", stdout);
        freopen_s((FILE**)stdin, "CONIN$", "rt", stdin);
    }
#ifdef _DEBUG
    cRendererDebugContext& DebugContext = cRendererDebugContext::GetInstance();
    DebugContext.Initialize();
#endif

    if (!Renderer.InitHWMngr()) {
        std::cout << "Failed to Init HardWareManager\n";
        return false;
    }
    if (!Renderer.CreateDevice(0)) {
        std::cout << "Failed to Create D3D12 Device\n";
        return false;
    }
#ifdef _DEBUG
    if (!Renderer.InitDeviceDebug()) {
        std::cout << "Failed to Init D3D12 Device Debug\n";
        return false;
    }
#endif
    Log(TEXT("Renderer has been Created successfully.\n"));

    auto pQueue = Renderer.GetCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT);
    if (!pQueue) {
        std::cout << "Failed to Get Direct Command Queue\n";
        return false;
    }

	cSwapchainBuilder SwapchainBuilder(Renderer);
    cSwapchainBuilder::sSwapchainDesc1Setter Setter(SwapchainBuilder);
    Setter.InterfaceVersion = cSwapchainBuilder::eInterfaceVersion::INTERFACE_VERSION_1;
    Setter.SurfaceType = cSwapchainBuilder::eSurfaceType::SURFACE_TYPE_HWND;
    Setter.hWnd = hMainFrame;
    Setter.Size = Size;
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
        Log(TEXT("Failed to Create Swapchain\n"));
        return false;
    }
    Log(TEXT("Swapchain has been Created successfully.\n"));

    CreateRect();

    SetApp(this);
    bNeedResizeSwapchain = false;
    return true;
}

void cApp::Destroy() {
    DestroyRect();

    bool bNeedToLog = Swapchain.IsCreated();
    Swapchain.Destroy();
    if (bNeedToLog)
		Log(TEXT("Swapchain has been Destroyed.\n"));

    bNeedToLog = Renderer.GetDevice().IsCreated();
    Renderer.Destroy(true);
#ifdef _DEBUG
    cRendererDebugContext& DebugContext = cRendererDebugContext::GetInstance();
    DebugContext.Deinitialize();
#endif

    if (bNeedToLog)
		Log(TEXT("Renderer has been Destroyed.\n"));

    cBaseApp::Destroy();
}

void cApp::Run() {
    UINT PresentFlags;
    cFrameLimiter FrameLimiter;
    FrameLimiter.SetMaxFPS(0);

    cCommandQueue* pCommandQueue = Renderer.GetCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT);
    if (pCommandQueue == nullptr)
        return;
    cCommandQueue& CommandQueue = *pCommandQueue;
    
    cFrameSync FrameSync;
    if (!FrameSync.Create(Renderer.GetDevice(), Swapchain.GetBufferCount()))
        return;

    auto StartTime = std::chrono::system_clock::now();
	while (!IsExiting()) {
        PollMessage();
        if (bNeedResizeSwapchain) {
            FrameSync.Destroy();
            if (!ResizeSwapchain())
                break;
            if (!FrameSync.Create(Renderer.GetDevice(), Swapchain.GetBufferCount()))
                break;
        }
        FrameSync.InitGraphicsCommandList(Swapchain.GetCurrentBackBufferIndex());
        cGraphicsCommandList& CommandList = FrameSync.GetGraphicsCommandList(Swapchain.GetCurrentBackBufferIndex());

        if (!Swapchain.BeginFrame(CommandList)) {
            break;
        }
        if (Swapchain.GetCurrentRenderTargetDescriptor()) {
            const float ClearColor4f[] = {0.4f, 0.4f, 0.4f, 1.f};
            CommandList.ClearRenderTargetView(*Swapchain.GetCurrentRenderTargetDescriptor(), ClearColor4f, 0, nullptr);
        }
        CommandList.OMSetRenderTargetsDiscrete(1, Swapchain.GetCurrentRenderTargetDescriptor(), nullptr);

        DrawRect(CommandList);

        Swapchain.EndFrame(CommandList);

		CommandList.Close();
        CommandQueue.ExecuteCommandList(CommandList);

        int LastBackBufferIndex = -1;
        if (!bUseVsync) {
            PresentFlags = Renderer.IsSupportTearing() ? DXGI_PRESENT_ALLOW_TEARING : 0;
            LastBackBufferIndex = Swapchain.Present(0, PresentFlags);
        }
        else {
            LastBackBufferIndex = Swapchain.Present(1, 0);
        }
        if (LastBackBufferIndex < 0)
            break;
        FrameSync.Signal(CommandQueue, LastBackBufferIndex);

        FrameSync.WaitBackBuffer(Swapchain.GetCurrentBackBufferIndex());
        FrameLimiter.Wait();
    }
    Renderer.FlushCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT, Swapchain.GetBufferCount(),5000);
}

void cApp::SetFullscreen(bool _bFullscreen) {
    bFullscreen = _bFullscreen;

    DWORD Style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD ExStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
    if (bFullscreen) {
        Style = WS_POPUP | WS_VISIBLE;
        ExStyle = WS_EX_APPWINDOW;
    }

    SetWindowLong(hMainFrame, GWL_STYLE, Style);
    SetWindowLong(hMainFrame, GWL_EXSTYLE, ExStyle);

    if (bFullscreen) {
        HMONITOR hMonitor = MonitorFromWindow(hMainFrame, MONITOR_DEFAULTTONEAREST);
        MONITORINFO Info;
        Info.cbSize = sizeof(Info);
        if (GetMonitorInfo(hMonitor, &Info)) {
            SetWindowPos(
                hMainFrame,
                nullptr,
                Info.rcMonitor.left,
                Info.rcMonitor.top,
                Info.rcMonitor.right - Info.rcMonitor.left,
                Info.rcMonitor.bottom - Info.rcMonitor.top,
                SWP_NOZORDER
            );
        }
    }
	else {
        ShowWindow(hMainFrame, SW_MAXIMIZE);
    }
}

void cApp::Log(LPCTSTR Msg) const {
#ifdef UNICODE
    std::wcout << Msg;
#else
    std::cout << Msg;
#endif
}

void cApp::OnSize(UINT nType, UINT Width, UINT Height) {
    if (Width < 8 || Height < 8 || (Size.cx == Width && Size.cy == Height)) {
        return;
    }

    Size.cx = Width;
    Size.cy = Height;
    bNeedResizeSwapchain = true;
}

bool cApp::ResizeSwapchain() {
    Renderer.FlushCommandQueue(cDevice::eCommandType::COMMAND_TYPE_DIRECT, Swapchain.GetBufferCount(), 5000);
    if (!Swapchain.Resize(Renderer.GetDevice(), Swapchain.GetBufferCount(), Size, Swapchain.GetFormat(), Swapchain.GetFlags())) {
        assert(false);
        return false;
    }
    bNeedResizeSwapchain = false;
    return true;
}

LRESULT cApp::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_SIZE:
        if (GetApp()) {
            UINT Width = LOWORD(lParam);
            UINT Height = HIWORD(lParam);

            ((cApp*)GetApp())->OnSize((UINT)wParam, Width, Height);
        }
        return 0;
    case WM_KEYDOWN:
        if (GetApp() && wParam == VK_F11) {
            bool bFullscreen = ((cApp*)GetApp())->IsFullscreen();
            ((cApp*)GetApp())->SetFullscreen(!bFullscreen);
        }
        return 0;
    case WM_MOUSEMOVE:
        if (GetApp()) {
            ((cApp*)GetApp())->RectPos.x = LOWORD(lParam);
            ((cApp*)GetApp())->RectPos.y = HIWORD(lParam);
	    }
        return 0;
    }
    return cBaseApp::WindowProc(hWnd, uMsg, wParam, lParam);
}

void cApp::CreateRect() {
    const float sin60 = sin(sqrt(3.0) * 0.5);

    const sVertex RectVertexData[] = {
        {glm::vec3(-0.5,-0.5, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 1.0), glm::vec2(), glm::vec4(1.0), glm::u16vec4(), glm::vec4(0.0)},
        {glm::vec3(-0.5, 0.5, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 0.0), glm::vec2(), glm::vec4(1.0), glm::u16vec4(), glm::vec4(0.0)},
        {glm::vec3(0.5,  0.5, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(1.0, 0.0, 0.0), glm::vec2(1.0, 0.0), glm::vec2(), glm::vec4(1.0), glm::u16vec4(), glm::vec4(0.0)},
        {glm::vec3(0.5, -0.5, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(1.0, 0.0, 0.0), glm::vec2(1.0, 1.0), glm::vec2(), glm::vec4(1.0), glm::u16vec4(), glm::vec4(0.0)},
    };
    const UINT RectVertexIndex[] = {
        1, 3, 0,
        1, 2, 3,
    };

    std::vector<sVertex> Vertices(4);
    std::vector<UINT>   Indecies(6);
    memcpy(Vertices.data(), RectVertexData, sizeof(RectVertexData));
    memcpy(Indecies.data(), RectVertexIndex, sizeof(RectVertexIndex));
    bool bResult = RectPrimitive.Create(Renderer, Vertices, Indecies);


    MintChoco::cImage RectTextureImage;
    RectTextureImage.Destroy();
    bResult = cImageLoader::Load(RectTextureImage, (ProcessDiectory + TEXT("Texture.jpg")).c_str());

    bResult = RectTexture.CreateFromImage(Renderer, RectTextureImage);

    cResourceFactory Factory(Renderer.GetDevice());
    bResult = Factory.CreateUploadBuffer(RectTransform, sizeof(sTransform) * Swapchain.GetBufferCount());

    RectDescripotrHeap[0].Create(Renderer.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    RectDescripotrHeap[1].Create(Renderer.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    RectDescripotrHeap[2].Create(Renderer.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = RectTexture.GetBuffer().GetDescription1().Format;
    SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = 1;
    SRVDesc.Texture2D.PlaneSlice = 0;
    SRVDesc.Texture2D.ResourceMinLODClamp = 0;

    D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
    CBVDesc.SizeInBytes = sizeof(sTransform);

    for (size_t iBuffer = 0; iBuffer < Swapchain.GetBufferCount(); ++iBuffer) {
        CBVDesc.BufferLocation = RectTransform.GetResouce()->GetGPUVirtualAddress() + sizeof(sTransform) * iBuffer;
        RectDescripotrHeap[iBuffer].CreateConstantBufferView(Renderer.GetDevice(), 0, CBVDesc);

        RectDescripotrHeap[iBuffer].CreateShaderResourceView(Renderer.GetDevice(), RectTexture.GetBuffer(), 1, SRVDesc);
    }

    auto Token = RectTransform.Map(0, sizeof(sTransform) * Swapchain.GetBufferCount());
    RectTransformBuffer = (BYTE*)Token.Ptr;

    LoadRectShader();
}

void cApp::LoadRectShader() {
    WCHAR _RootSignature[] = TEXT("TriangleRS.cso");
    WCHAR _Vertex[] = TEXT("TriangleVS.cso");
    WCHAR _Pixel[] = TEXT("TrianglePS.cso");

    std::wstring Root = ProcessDiectory + _RootSignature;
    std::wstring Vertex = ProcessDiectory + _Vertex;
    std::wstring Pixel = ProcessDiectory + _Pixel;

    Shader[0].Create(Root);
    Shader[1].Create(Vertex);
    Shader[2].Create(Pixel);

    RectRootSignature.Create(Renderer.GetDevice(), Shader[0].GetD3D12ShaderByteCode());

    D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc;
    // Input Layout
    Desc.pRootSignature = RectRootSignature.GetRootSignature();
    Desc.InputLayout.NumElements = sVertex::GetAttributeCount();
    Desc.InputLayout.pInputElementDescs = sVertex::InputElement;
    Desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    Desc.VS = Shader[1].GetD3D12ShaderByteCode();
    Desc.PS = Shader[2].GetD3D12ShaderByteCode();
    Desc.DS = {};
    Desc.GS = {};
    Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    Desc.HS = {};
    // Rasterizer
    Desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    Desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    Desc.RasterizerState.FrontCounterClockwise = FALSE;
    Desc.RasterizerState.DepthBias = 0;
    Desc.RasterizerState.DepthBiasClamp = 0;
    Desc.RasterizerState.SlopeScaledDepthBias = 0;
    Desc.RasterizerState.DepthClipEnable = FALSE;
    Desc.RasterizerState.MultisampleEnable = FALSE;
    Desc.RasterizerState.AntialiasedLineEnable = FALSE;
    Desc.RasterizerState.ForcedSampleCount = 0;
    Desc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    // Stream Output
    Desc.StreamOutput = {};
    Desc.NodeMask = 0;
    Desc.CachedPSO = {};
    Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    Desc.NumRenderTargets = 1;
    Desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    for (size_t iRT = 1; iRT < 8; ++iRT) Desc.RTVFormats[iRT] = DXGI_FORMAT_UNKNOWN;
    Desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    Desc.BlendState.AlphaToCoverageEnable = FALSE;
    Desc.BlendState.IndependentBlendEnable = FALSE;
    Desc.BlendState.RenderTarget[0].BlendEnable = TRUE;
    Desc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
    Desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
    Desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
    Desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    Desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
    Desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    Desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    Desc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    Desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    Desc.DepthStencilState.DepthEnable = FALSE;
    Desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    Desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    Desc.DepthStencilState.StencilEnable = FALSE;
    Desc.DepthStencilState.StencilReadMask = 0;
    Desc.DepthStencilState.StencilWriteMask = 0;
    Desc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    Desc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    Desc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    Desc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    Desc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    Desc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    Desc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    Desc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    Desc.SampleMask = 0xFFFFFFFF;
    Desc.SampleDesc.Count = 1;
    Desc.SampleDesc.Quality = 0;

    RectPipelineStateObject.Create(Renderer.GetDevice(), Desc);
}

void cApp::DestroyRect() {
    RectPipelineStateObject.Destroy();

    RectRootSignature.Destroy();
    for (size_t iShader = 0; iShader < 3; ++iShader) {
        Shader[iShader].Destroy();
    }

    for (size_t iBuffer = 0; iBuffer < Swapchain.GetBufferCount(); ++iBuffer) {
		RectDescripotrHeap[iBuffer].Destroy();
    }
    RectTexture.Destroy();
    RectPrimitive.Destroy();
}

void cApp::DrawRect(cGraphicsCommandList& CmdList) {
    const size_t iCurrentBuffer = Swapchain.GetCurrentBackBufferIndex();

    CmdList.GetGraphicsCommandList()->SetPipelineState(RectPipelineStateObject.GetPipelineState());
    CmdList.GetGraphicsCommandList()->SetGraphicsRootSignature(RectRootSignature.GetRootSignature());
    CmdList.SetDescriptorHeap(RectDescripotrHeap[iCurrentBuffer]);

    CmdList.GetGraphicsCommandList()->IASetVertexBuffers(0, 1, &RectPrimitive.GetVertexBufferView());
    CmdList.GetGraphicsCommandList()->IASetIndexBuffer(&RectPrimitive.GetIndexBufferView());
    CmdList.GetGraphicsCommandList()->IASetPrimitiveTopology(RectPrimitive.GetTopology());

    D3D12_VIEWPORT ViewPort;
    ViewPort.Width = Size.cx;
    ViewPort.Height = Size.cy;
    ViewPort.MaxDepth = 0.0f;
    ViewPort.MinDepth = 1.0f;
    ViewPort.TopLeftX = 0.0;
    ViewPort.TopLeftY = 0.0;
    CmdList.GetGraphicsCommandList()->RSSetViewports(1, &ViewPort);

    D3D12_RECT ScissorRect;
    ScissorRect.left = ScissorRect.top = 0;
    ScissorRect.right = Size.cx;
    ScissorRect.bottom = Size.cy;
    CmdList.GetGraphicsCommandList()->RSSetScissorRects(1, &ScissorRect);

    static auto StartTime = std::chrono::system_clock::now();
    auto CurrentTime = std::chrono::system_clock::now();
    float t = std::chrono::duration_cast<std::chrono::duration<double>>(CurrentTime - StartTime).count();
    t = sin(t * 5.0) * 0.5 + 0.5;
    const float Color[3] = {t, 0, 0};
    CmdList.GetGraphicsCommandList()->SetGraphicsRoot32BitConstants(0, 3, Color, 0);
    CmdList.GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(1, *RectDescripotrHeap[iCurrentBuffer].GetGPUDescriptorHandle(0));

    // transform
    glm::vec2 Pos = RectPos;
    Pos.y = Swapchain.GetSize().Height - Pos.y;
    Pos.x -= Swapchain.GetSize().Width * 0.5f;
    Pos.y -= Swapchain.GetSize().Height * 0.5f;
    sTransform Transform;
    Transform.Model         = glm::translate(glm::mat4(1.), glm::vec3(Pos, 0.0));
	//Transform.Model         = glm::rotate(Transform.Model, RectPos.x * 0.01f,glm::vec3(0, 0, -1));
	Transform.Model         = glm::scale(Transform.Model, glm::vec3(400));

    Transform.View          = glm::lookAtLH(glm::vec3(0, 0, -5), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
    Transform.InverseView   = glm::inverse(Transform.View);
    Transform.Projection    = glm::orthoLH_ZO(
        -Swapchain.GetSize().Width / 2.0f, Swapchain.GetSize().Width / 2.0f,
        -Swapchain.GetSize().Height / 2.0f, Swapchain.GetSize().Height / 2.0f,
        1.0f, 1000.f);
    memcpy(RectTransformBuffer + sizeof(sTransform) * iCurrentBuffer, &Transform, sizeof(sTransform));

    CmdList.GetGraphicsCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
