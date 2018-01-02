#pragma once

#include "Input.h"
#include <d3d11.h>
#include <d3dx11.h>
#include "text2D.h"

enum BUTTONS
{	
	eQuit,
	eStartGame,
	eNeutral

};

class MenuSystem
{
private:

	Text2D*					m_pStart;
	Text2D*					m_pQuit;

	ID3D11Device*           m_pD3DDevice;
	ID3D11DeviceContext*    m_pImmediateContext;

	
	int		m_Position;
	bool	m_Selection;

public:

	BUTTONS		m_ePlayerSelection;

	MenuSystem(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext);
	~MenuSystem();

	//Initializes the menu
	void SetupMainMenu();

	void MainMenuLoop(Input* player); // Menu at the beginning of the game
	void PauseMenu(Input* player); // Pause menu for in game
	void RenderMenu(); 
	bool IsHighlighted(BUTTONS Button);
	bool GetSelection();

};

