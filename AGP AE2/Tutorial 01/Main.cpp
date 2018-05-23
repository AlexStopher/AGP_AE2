#include <windows.h>

#include "GameManager.h"
#define _XM_NO_INTRINSICS
#define XM_NO_ALIGNMENT

//Global variables 



char	  g_TutorialName[100] = "Tutorial 07 Exercise 01\0";


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	
	GameManager* p_Game = new GameManager;


	if (FAILED(p_Game->InitaliseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	

	if (FAILED(p_Game->m_pPlayerInput->InitialiseInput(p_Game->GetHInstance(), p_Game->GetHWnd())))
	{
		DXTRACE_MSG("Failed to create Input");
		return 0;
	}


	MSG msg = { 0 };

	if (FAILED(p_Game->InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}


	if (FAILED(p_Game->InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}

	while (msg.message != WM_QUIT)
	{
		


		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(p_Game->m_eGameState == eMainMenu)
		{
			p_Game->MainMenu();
		}
		else if(p_Game->m_eGameState == eInGame)
		{
			p_Game->GameLogic();
			p_Game->RenderFrame();
		}
		else if (p_Game->m_eGameState == ePauseMenu)
		{
			//Pause menu
			p_Game->PauseMenu();
		}
		else if (p_Game->m_eGameState == eLoading)
		{
			p_Game->LoadingScreen();
		}

		if (p_Game->m_eGameState == eEndGame)
		{
			PostQuitMessage(0);
		}

	}

	p_Game->ShutdownD3D();
	return (int)msg.wParam;

}
