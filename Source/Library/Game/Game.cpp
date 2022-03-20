#include "Game/Game.h"


namespace library
{
    /*--------------------------------------------------------------------
      Global Variables
    --------------------------------------------------------------------*/
    using namespace Microsoft::WRL;

    HINSTANCE               g_hInst = nullptr; // 인스턴스 핸들
    HWND                    g_hWnd = nullptr; // 윈도우 핸들
    D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
    //ID3D11Device* g_pd3dDevice = nullptr;
    ComPtr<ID3D11Device> g_pd3dDevice;

    //ID3D11Device1* g_pd3dDevice1 = nullptr;
    ComPtr<ID3D11Device1> g_pd3dDevice1;

    //ID3D11DeviceContext* g_pImmediateContext = nullptr;
    ComPtr<ID3D11DeviceContext> g_pImmediateContext;

    //ID3D11DeviceContext1* g_pImmediateContext1 = nullptr;
    ComPtr<ID3D11DeviceContext1> g_pImmediateContext1;

    //IDXGISwapChain* g_pSwapChain = nullptr;
    ComPtr<IDXGISwapChain> g_pSwapChain;

    //IDXGISwapChain1* g_pSwapChain1 = nullptr;
    ComPtr<IDXGISwapChain1> g_pSwapChain1;

    //ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
    ComPtr<ID3D11RenderTargetView> g_pRenderTargetView;

    //IDXGIFactory1* dxgiFactory = nullptr;
    ComPtr<IDXGIFactory1> dxgiFactory;
    

    //IDXGIFactory2* dxgiFactory2 = nullptr;
    ComPtr<IDXGIFactory2> dxgiFactory2;
    

    /*--------------------------------------------------------------------
      Forward declarations
    --------------------------------------------------------------------*/

    /*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      Function: WindowProc

      Summary:  Defines the behavior of the window—its appearance, how
                it interacts with the user, and so forth

      Args:     HWND hWnd
                  Handle to the window
                UINT uMsg
                  Message code
                WPARAM wParam
                  Additional data that pertains to the message
                LPARAM lParam
                  Additional data that pertains to the message

      Returns:  LRESULT
                  Integer value that your program returns to Windows
    -----------------------------------------------------------------F-F*/


    LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_SIZE:
        {
            //int width = LOWORD(lParam);  // Macro to get the low-order word.
            //int height = HIWORD(lParam); // Macro to get the high-order word.
            // Respond to the message:
            //OnSize(hWnd, (UINT)wParam, width, height);
        }
        break;
        }
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow)
    {
        // Register class
        WNDCLASSEX wcex; // 윈도우 클래스 선언
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowProc; // 함수 포인터. OS에서 발생하는 이벤트(키보드, 마우스 등)들이 발생할 때 알려달라는 것
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1); // 
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // null 넣으면 알아서 찾음
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"TutorialWindowClass";
        wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
        if (!RegisterClassEx(&wcex)) // 윈도우 클래스 등록 , 오류시 탈출
            return E_FAIL;

        // Create window
        g_hInst = hInstance;
        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE); // 작업영역, 윈도우 스타일, 메뉴여부
        g_hWnd = CreateWindow(L"TutorialWindowClass", L"Game Graphics Programming Lab 01: Direct3D 11 Basics", /// wide character 이기 때문에 L을 붙임
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
            nullptr);
        if (!g_hWnd) // 오류 검사
            return E_FAIL;

        ShowWindow(g_hWnd, nCmdShow);//창띄우기

        return S_OK;
    }

    HRESULT InitDevice()
    {
        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(g_hWnd, &rc);
        UINT width = (UINT(rc.right - rc.left)); // unsigned 또는 signed가 일치하지 않습니다. 수정
        UINT height = (UINT(rc.bottom - rc.top));

        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            g_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, g_pd3dDevice.GetAddressOf(), &g_featureLevel, g_pImmediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, g_pd3dDevice.GetAddressOf(), &g_featureLevel, g_pImmediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr))
                break;
        }

        if (FAILED(hr))
            return hr;

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
         
       // ComPtr<IDXGIFactory1> dxgiFactory;
        
        {
        //IDXGIDevice* dxgiDevice = nullptr;
            ComPtr<IDXGIDevice> dxgiDevice;
            //hr = g_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgiDevice.GetAddressOf()));
            //if (SUCCEEDED(hr))
            if (SUCCEEDED(g_pd3dDevice.As(&dxgiDevice)))
            {
                //IDXGIAdapter* adapter = nullptr;
                ComPtr<IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
                    //adapter->Release();
                }
                //dxgiDevice->Release();
            }
        }
        if (FAILED(hr))
            return hr;

        

        // Create swap chain
        
        //ComPtr<IDXGIFactory2> dxgiFactory2;
        
        if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
        {
            // DirectX 11.1 or later
            //hr = g_pd3dDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(g_pd3dDevice1.GetAddressOf()));
            //if (SUCCEEDED(hr))
            if(SUCCEEDED(g_pd3dDevice.As(&g_pd3dDevice1)))
            {
                //(void)g_pImmediateContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(g_pImmediateContext1.GetAddressOf()));
                (void)g_pImmediateContext.As(&g_pImmediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd = {};
            sd.Width = width;
            sd.Height = height;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice.Get(), g_hWnd, &sd, nullptr, nullptr, g_pSwapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                g_pSwapChain1.As(&g_pSwapChain);
            }
            //dxgiFactory2->Release();
        }
        else
        {
        }
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd = {};
            sd.BufferCount = 1;
            sd.BufferDesc.Width = width;
            sd.BufferDesc.Height = height;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = g_hWnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;

            if (SUCCEEDED(hr))
            {
                hr = dxgiFactory->CreateSwapChain(g_pd3dDevice.Get(), &sd, g_pSwapChain.GetAddressOf());
            }
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);

        //dxgiFactory->Release();

        if (FAILED(hr))
            return hr;

        // Create a render target view
        //ID3D11Texture2D* pBackBuffer = nullptr;
        ComPtr<ID3D11Texture2D> pBackBuffer;
        hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
        if (FAILED(hr))
            return hr;

        hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, g_pRenderTargetView.GetAddressOf());
        //pBackBuffer->Release();
        if (FAILED(hr))
            return hr;

        g_pImmediateContext->OMSetRenderTargets(1, g_pRenderTargetView.GetAddressOf(), nullptr);

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = static_cast<FLOAT>(width);
        vp.Height = static_cast<FLOAT>(height);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        g_pImmediateContext->RSSetViewports(1, &vp);

        return S_OK;
    }


    //--------------------------------------------------------------------------------------
    // Render the frame
    //--------------------------------------------------------------------------------------
    void Render()
    {
        // Just clear the backbuffer

        g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), Colors::MidnightBlue);
        g_pSwapChain->Present(0, 0);
        
    }


    //--------------------------------------------------------------------------------------
    // Clean up the objects we've created
   //--------------------------------------------------------------------------------------
    
    void CleanupDevice()
    {
        //if (g_pImmediateContext) g_pImmediateContext->ClearState();
        //if (g_pRenderTargetView) g_pRenderTargetView->Release();
        //if (g_pSwapChain1) g_pSwapChain1->Release();
        //if (g_pSwapChain) g_pSwapChain->Release();
        //if (g_pImmediateContext1) g_pImmediateContext1->Release();
        //if (g_pImmediateContext) g_pImmediateContext->Release();
        //if (g_pd3dDevice1) g_pd3dDevice1->Release();
        //if (g_pd3dDevice) g_pd3dDevice->Release();
    }
}