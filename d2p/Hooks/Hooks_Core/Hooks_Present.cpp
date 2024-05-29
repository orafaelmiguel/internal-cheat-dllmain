#include <D3DX11.h>
#include <d3d11.h>
#include "../../Menu/Menu.h"
#include "../../ImGui/imgui_impl_dx11.h"
#include "../../ImGui/imgui_impl_win32.h"
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#include "../Minhook/MinHook.h"
#include "./Hooks_Present.h"

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);


typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


Present oPresent;

HWND window = NULL;

WNDPROC oWndProc;

ID3D11Device* pDevice = NULL;

ID3D11DeviceContext* pContext = NULL;

ID3D11RenderTargetView* mainRenderTargetView;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    if (Menu::isVisible)
    {
        WM_RBUTTONDOWN;
        return 1;
    }
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!KieroHooks::Imgui_Init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);

            DXGI_SWAP_CHAIN_DESC sd;

            pSwapChain->GetDesc(&sd);

            window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer;

            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);

            pBackBuffer->Release();

            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

            KieroHooks::InitImgui();

            Menu::Style();

            KieroHooks::Imgui_Init = true;

        }
        else
            return oPresent(pSwapChain, SyncInterval, Flags);
    }

    static bool fisr = true;
    if (fisr)
    {


        fisr = false;
    }

    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        Menu::isVisible = !Menu::isVisible;

    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if (Menu::isVisible)
    {
        Menu::Render();
    }


    ImGui::EndFrame();
    ImGui::Render();

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}



void KieroHooks::Init_Menu()
{

    bool should_load = false;
    do
    {

        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            kiero::bind(8, (void**)&oPresent, hkPresent);
            should_load = true;
        }

    } while (should_load == false);

}

void KieroHooks::Shutdown_Menu()
{
    MH_DisableHook(MH_ALL_HOOKS);
}

void KieroHooks::InitImgui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);
    ImGui::GetIO().MouseDrawCursor = true;
}
