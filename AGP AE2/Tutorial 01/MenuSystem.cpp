#include "MenuSystem.h"


MenuSystem::MenuSystem(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext)
{
	m_pD3DDevice = D3DDevice;
	m_pImmediateContext = ImmediateContext;
}

MenuSystem::~MenuSystem()
{
	m_pStart->~Text2D();
	m_pQuit->~Text2D();
	m_pTitle->~Text2D();
	m_pD3DDevice->Release();
	m_pImmediateContext->Release();

}

void MenuSystem::SetupMainMenu()
{
	m_pStart = new Text2D("assets/font1.png", m_pD3DDevice, m_pImmediateContext);
	m_pQuit = new Text2D("assets/font1.png", m_pD3DDevice, m_pImmediateContext);
	m_pTitle = new Text2D("assets/font1.png", m_pD3DDevice, m_pImmediateContext);
}

void MenuSystem::MainMenuLoop(Input* player)
{
	

	player->ReadInputStates();

	m_pTitle->AddText("DUUM", -0.8f, 0.7f, 0.1f);
	m_pStart->AddText("Start Game", -0.7f, 0.3f, 0.1f);
	m_pQuit->AddText("Quit", -0.7f, 0.0f, 0.1f);



	//if statements for the players input to move the position of the highlighted item
	if ((player->IsKeyPressed(DIK_W) || player->IsButtonPressed(XINPUT_GAMEPAD_DPAD_UP)) 
		&& m_Position < 1)
	{
		m_Position++;
	}
	else if ((player->IsKeyPressed(DIK_S) || player->IsButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN))
		&& m_Position > 0)
	{
		m_Position--;
	}

	//if true then change the size of the box to show it is being highlighted	
	if (IsHighlighted(eStartGame))
	{
		m_pStart->AddText("Start Game", -0.7f, 0.3f, 0.15f);
		
	}
	else if (IsHighlighted(eQuit))
	{
		m_pQuit->AddText("Quit", -0.7f, 0.0f, 0.15f);
	}


	//Statement below checks to see if Cross was pressed and if so changes the selection value to true
	//else it reverts it to false on the next loop;
	if (player->IsKeyPressed(DIK_SPACE)
		|| player->IsButtonPressed(XINPUT_GAMEPAD_A))
	{
		m_Selection = true;
	}
	else
	{
		m_Selection = false;
	}

	if (IsHighlighted(eStartGame) && m_Selection == true)
	{
		m_ePlayerSelection = eStartGame;

	}
	else if (IsHighlighted(eQuit) && m_Selection == true)
	{
		m_ePlayerSelection = eQuit;
	}

	m_pStart->RenderText();
	m_pQuit->RenderText();
	m_pTitle->RenderText();

}

void MenuSystem::PauseMenu(Input* player)
{
	m_Selection = false;

	player->ReadInputStates();

	m_pStart->AddText("Continue", -0.7f, 0.3f, 0.1f);
	m_pQuit->AddText("Quit", -0.7f, 0.0f, 0.1f);

	if ((player->IsKeyPressed(DIK_W) || player->IsButtonPressed(XINPUT_GAMEPAD_DPAD_UP))
		&& m_Position < 1)
	{
		m_Position++;
	}
	else if (player->IsKeyPressed(DIK_S) || player->IsButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN)
		&& m_Position > 0)
	{
		m_Position--;
	}

	if (player->IsKeyPressed(DIK_SPACE) 
		|| player->IsButtonPressed(XINPUT_GAMEPAD_A))
	{
		m_Selection = true;
	}
	else
	{
		m_Selection = false;
	}

	if (IsHighlighted(eStartGame))
	{
		m_pStart->AddText("Continue", -0.7f, 0.3f, 0.15f);

	}
	else if (IsHighlighted(eQuit))
	{
		m_pQuit->AddText("Quit", -0.7f, 0.0f, 0.15f);
	}

	if (IsHighlighted(eStartGame) && m_Selection == true)
	{
		m_ePlayerSelection = eStartGame;

	}
	else if (IsHighlighted(eQuit) && m_Selection == true)
	{
		m_ePlayerSelection = eQuit;
	}


	m_pStart->RenderText();
	m_pQuit->RenderText();
}

void MenuSystem::RenderMenu()
{
	


}

bool MenuSystem::IsHighlighted(BUTTONS Button)
{
	if (m_Position == Button)
		return true;

	return false;
}

bool MenuSystem::GetSelection()
{
	return m_Selection;
}