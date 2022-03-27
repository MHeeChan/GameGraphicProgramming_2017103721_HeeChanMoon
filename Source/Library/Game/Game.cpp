#include "Game/Game.h"

namespace library
{
	
	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Game::Game

	  Summary:  Constructor

	  Args:     PCWSTR pszGameName
				  Name of the game

	  Modifies: [m_pszGameName, m_mainWindow, m_renderer].
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	/*--------------------------------------------------------------------
	  TODO: Game::Game definition (remove the comment)
	--------------------------------------------------------------------*/
	Game::Game(_In_ PCWSTR pszGameName)
	{
		m_mainWindow = std::make_unique<MainWindow>();
		m_renderer = std::make_unique<Renderer>();
		m_pszGameName = pszGameName;
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Game::Initialize

	  Summary:  Initializes the components of the game

	  Args:     HINSTANCE hInstance
				  Handle to the instance
				INT nCmdShow
				  Is a flag that says whether the main application window
				  will be minimized, maximized, or shown normally

	  Modifies: [m_mainWindow, m_renderer].

	  Returns:  HRESULT
				Status code
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	HRESULT Game::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
	{
		// m_mainWindow -> MainWindow::MainWindow();
		m_mainWindow->Initialize(hInstance, nCmdShow, m_pszGameName);
		//HWND hWnd = m_mainWindow->GetWindow();
		m_renderer->Initialize(m_mainWindow->GetWindow());
		
		//initialize
		return S_OK;
		
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Game::Run

	  Summary:  Runs the game loop

	  Returns:  INT
				  Status code to return to the operating system
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	INT Game::Run()
	{
		MSG msg = {0};
		// msg.hwnd -> 메시지가 발생한 윈도우
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				m_renderer -> Render();
			}
		}
		return 0;
	}

	/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
	  Method:   Game::GetGameName

	  Summary:  Returns the name of the game

	  Returns:  PCWSTR
				  Name of the game
	M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
	PCWSTR Game::GetGameName() const
	{
		return m_pszGameName;
	}
}
