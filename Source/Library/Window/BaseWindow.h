
/*+===================================================================
  File:      BASEWINDOW.H

  Summary:   BaseWindow header file contains declarations of the
             base class of all windows used in the library.

  Classes: BaseWindow<DerivedType>

  © 2022 Kyung Hee University
===================================================================+*/
#pragma once

#include "Common.h"
#include <cassert>

namespace library
{
    /*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
      Class:    BaseWindow

      Summary:  An abstract base class from specific window

      Methods:  WindowProc
                  The window procedure of the window
                Initialize
                    Purely virtual function that initializes window
                GetWindowClassName
                    Purely virtual function that returns the name of
                    the window class
                HandleMessage
                    Purely virtual function that that handles the
                    messages
                GetWindow
                    Getter for the handle to the window
                BaseWindow
                    Constructor.
                ~BaseWindow
                    Destructor.
    C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
    template <class DerivedType>
    class BaseWindow
    {
    public:
        static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

        BaseWindow();
        BaseWindow(const BaseWindow& rhs) = delete;
        BaseWindow(BaseWindow&& rhs) = delete;
        BaseWindow& operator=(const BaseWindow& rhs) = delete;
        BaseWindow& operator=(BaseWindow&& rhs) = delete;
        virtual ~BaseWindow() = default;

        virtual HRESULT Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName) = 0;
        virtual PCWSTR GetWindowClassName() const = 0;
        virtual LRESULT HandleMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) = 0;

        HWND GetWindow() const;


    protected:
        HRESULT initialize(
            _In_ HINSTANCE hInstance,
            _In_ INT nCmdShow,
            _In_ PCWSTR pszWindowName,
            _In_ DWORD dwStyle,
            _In_opt_ INT x = CW_USEDEFAULT,
            _In_opt_ INT y = CW_USEDEFAULT,
            _In_opt_ INT nWidth = CW_USEDEFAULT,
            _In_opt_ INT nHeight = CW_USEDEFAULT,
            _In_opt_ HWND hWndParent = nullptr,
            _In_opt_ HMENU hMenu = nullptr
        );

        HINSTANCE m_hInstance;
        HWND m_hWnd;
        LPCWSTR m_pszWindowName;
    };

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
        Method:   BaseWindow<DerivedType>::WindowProc

        Summary:  Defines the behavior of the window—its appearance, how
                it interacts with the user, and so forth

        Args:     HWND hWnd
                    Handle to the window
                UINT uMessage
                    Message code
                WPARAM wParam
                    Additional data the pertains to the message
                LPARAM lParam
                    Additional data the pertains to the message

        Modifies: [m_hWnd].

        Returns:  LRESULT
                    Integer value that your program returns to Windows
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: BaseWindow<DerivedType>::WindowProc definition (remove the comment)
    --------------------------------------------------------------------*/
    template <class DerivedType>
    LRESULT BaseWindow<DerivedType>::WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        DerivedType* pThis = nullptr;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pThis = reinterpret_cast<DerivedType*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
            pThis->m_hWnd = hWnd;
        }
        else
        {
            pThis = reinterpret_cast<DerivedType*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
        Method:   BaseWindow<DerivedType>::BaseWindow

        Summary:  Constructor

        Modifies: [m_hInstance, m_hWnd, m_pszWindowName].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: BaseWindow<DerivedType>::BaseWindow definition (remove the comment)
    --------------------------------------------------------------------*/
    template <class DerivedType>
    BaseWindow<DerivedType>::BaseWindow():m_hInstance(nullptr), m_hWnd(nullptr), m_pszWindowName(nullptr) {}
   

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
        Method:   BaseWindow<DerivedType>::GetWindow()

        Summary:  Returns the handle to the window

        Returns:  HWND
                    The handle to the window
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: BaseWindow<DerivedType>::GetWindow definition (remove the comment)
    --------------------------------------------------------------------*/
    template <class DerivedType>
    HWND BaseWindow<DerivedType>::GetWindow() const
    {
        return m_hWnd;
    }




    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   BaseWindow<DerivedType>::initialize

      Summary:  Registers the window class and creates a window

      Args:     HINSTANCE hInstance
                  Handle to the instance
                INT nCmdShow
                  Is a flag that says whether the main application window
                  will be minimized, maximized, or shown normally
                PCWSTR pszWindowName
                  The window name
                DWORD dwStyle
                  The style of the window being created
                INT x
                  The initial horizontal position of the window
                INT y
                  The initial vertical position of the window
                INT nWidth
                  The width, in device units, of the window
                INT nHeight
                  The height, in device units, of the window
                HWND hWndParent
                  A handle to the parent or owner window of the window
                  being created
                HMENU hMenu
                  A handle to a menu, or specifies a child-window
                  identifier depending on the window style

      Modifies: [m_hInstance, m_pszWindowName, m_hWnd].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: BaseWindow<DerivedType>::initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    template <class DerivedType>
    HRESULT BaseWindow<DerivedType>::initialize(_In_ HINSTANCE hInstance, // register class
        _In_ INT nCmdShow,
        _In_ PCWSTR pszWindowName,
        _In_ DWORD dwStyle,
        _In_opt_ INT x,
        _In_opt_ INT y,
        _In_opt_ INT nWidth,
        _In_opt_ INT nHeight,
        _In_opt_ HWND hWndParent,
        _In_opt_ HMENU hMenu)
    {
        m_pszWindowName = pszWindowName;
        WNDCLASSEX wcex; // 윈도우 클래스 선언
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowProc; // 함수 포인터. OS에서 발생하는 이벤트(키보드, 마우스 등)들이 발생할 때 알려달라는 것
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL); // 
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // null 넣으면 알아서 찾음
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = GetWindowClassName();
        wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL);
 
        if (!RegisterClassEx(&wcex)) // 윈도우 클래스 등록 , 오류시 탈출
            return E_FAIL;
            
        m_hInstance = hInstance;
        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE); // 작업영역, 윈도우 스타일, 메뉴여부
        m_hWnd = CreateWindow(GetWindowClassName(), pszWindowName, /// wide character 이기 때문에 L을 붙임
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, this);


        if(m_hWnd==NULL)
        {
            DWORD dwError = GetLastError();
            MessageBox(
                nullptr,
                L"Call to RegisterClassEx failed!",
                L"Game Graphics Programming",
                NULL
            );
            if (dwError != ERROR_CLASS_ALREADY_EXISTS)
            {
                return HRESULT_FROM_WIN32(dwError);
            }
            return E_FAIL;
        }

        ShowWindow(m_hWnd, nCmdShow);//창띄우기

        return S_OK;
    }
}
