#include "MenuSystem.h"


MenuSystem::MenuSystem(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext)
{
	m_pD3DDevice = D3DDevice;
	m_pImmediateContext = ImmediateContext;
}

MenuSystem::~MenuSystem()
{

}

void MenuSystem::SetupMainMenu()
{
	m_pStart = new Text2D("assets/font1.png", m_pD3DDevice, m_pImmediateContext);
	m_pQuit = new Text2D("assets/font1.png", m_pD3DDevice, m_pImmediateContext);
	
}

void MenuSystem::MainMenuLoop(Input* player)
{
	m_pStart->AddText("ScaryZone", -0.6f, 0.6f, 0.2f);
	m_pQuit->AddText("NotScaryZone", -0.6f, 0.5f, 0.2f);

	//if statements for the players input to move the position of the highlighted item
	if (player->IsKeyPressed(DIK_W) && m_Position < 1)
	{
		m_Position++;
	}
	else if (player->IsKeyPressed(DIK_S) && m_Position > 0)
	{
		m_Position--;
	}

	//if true then change the size of the box to show it is being highlighted	
	if (IsHighlighted(eStartGame))
	{
	
		
	}
	else if (IsHighlighted(eQuit))
	{
		
	}


	//Statement below checks to see if Cross was pressed and if so changes the selection value to true
	//else it reverts it to false on the next loop;
	if (player->IsKeyPressed(DIK_SPACE))
	{
		m_Selection = true;
	}
	else
	{
		m_Selection = false;
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