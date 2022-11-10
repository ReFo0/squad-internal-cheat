#include <misc/includes.hpp>
#include <misc/util.h>
#include <misc/xor.h>
#include <misc/lazy.h>
#include <citos.h>

#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)         

typedef HRESULT(__stdcall* Present_t) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(__stdcall* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef BYTE _BYTE;
typedef DWORD _DWORD;
typedef unsigned __int64 _QWORD;

ID3D11RenderTargetView* RenderTarget;
ID3D11DeviceContext* Context;
ID3D11Device* Device;
Present_t OPresent;
WNDPROC oWndProc;
HWND Window = 0;

typedef __int64(__fastcall* thookFunction)(void* address, __int64 fnc, _QWORD* original, int a);
thookFunction hookFunction = nullptr;

int Width;
int Height;

namespace Globals
{
    static bool Open = true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN && LOWORD(wParam) == VK_HOME)
        Globals::Open ^= 1;

    ImGuiIO& io = ImGui::GetIO();
    POINT position;

    GetCursorPos(&position);
    ScreenToClient(Window, &position);
    io.MousePos.x = (float)position.x;
    io.MousePos.y = (float)position.y;

    if (Globals::Open)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        return true;
    }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}



HRESULT __fastcall hkPresentScene(IDXGISwapChain* pSwapChain, unsigned int SyncInterval, unsigned int Flags)
{
    static bool first = false;
    if (!first)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Device)))
        {
            Device->GetImmediateContext(&Context);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            Window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer;

            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            Device->CreateRenderTargetView(pBackBuffer, NULL, &RenderTarget);
            pBackBuffer->Release();

            oWndProc = (WNDPROC)SetWindowLongPtr(Window, GWLP_WNDPROC, (LONG_PTR)WndProc);

            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->AddFontFromFileTTF(_("C:\\Windows\\Fonts\\Tahoma.ttf"), 13.0f);
            ImGui_ImplWin32_Init(Window);
            ImGui_ImplDX11_Init(Device, Context);

            first = true;
        }
        else
            return OPresent(pSwapChain, SyncInterval, Flags);
    }

    if (Device || Context)
    {
        ID3D11Texture2D* renderTargetTexture = nullptr;
        if (!RenderTarget)
        {
            if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&renderTargetTexture))))
            {
                Device->CreateRenderTargetView(renderTargetTexture, nullptr, &RenderTarget);
                renderTargetTexture->Release();
            }
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    initcitos();
    if (Globals::Open)
    {
        ImGui::GetIO().MouseDrawCursor = 1;
        ImGui::SetWindowSize(ImVec2(230, 300));
        ImGui::Begin(_("squad"));
        ImGui::Checkbox(_("draw line"), &esp);

        ImGui::End();
    }
    else
    {
        ImGui::GetIO().MouseDrawCursor = 0;
    }

    ImGui::EndFrame();
    Context->OMSetRenderTargets(1, &RenderTarget, NULL);
    ImGui::Render();

    if (RenderTarget)
    {
        RenderTarget->Release();
        RenderTarget = nullptr;
    }

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return OPresent(pSwapChain, SyncInterval, Flags);
}

void hook(__int64 addr, __int64 func, __int64* orig)
{
    static uintptr_t hook_addr;
    if (!hook_addr)
        hook_addr = sig_scan(_("GameOverlayRenderer64.dll"), _("48 ? ? ? ? 57 48 83 EC 30 33 C0"));
    auto hook = ((__int64(__fastcall*)(__int64 addr, __int64 func, __int64* orig, __int64 smthng))(hook_addr));
    hook((__int64)addr, (__int64)func, orig, (__int64)1);
}

void init() {
    if (!SAFE_CALL(GetModuleHandleA)(_("GameOverlayRenderer64.dll")))
    {
        abort();
    }

    uintptr_t Steam_DXGI_PresentScene = sig_scan(_("GameOverlayRenderer64.dll"), _("48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 20 41 8B E8"));

    if (Steam_DXGI_PresentScene)
        hook(Steam_DXGI_PresentScene, (__int64)hkPresentScene, (__int64*)&OPresent);
}

bool __stdcall DllMain(HMODULE dll_module, DWORD conclusion, LPVOID reserved)
{
    UNREFERENCED_PARAMETER(dll_module);
    UNREFERENCED_PARAMETER(reserved);
    if (conclusion == 1)
    {
        init();

    }

    return TRUE;
}

