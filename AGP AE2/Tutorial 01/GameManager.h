#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include <iostream>
#include "Camera.h"
#include "text2D.h"
#include "Model.h"
#include "Input.h"
#include "SceneNode.h"
#include "MenuSystem.h"
#include "Math.h"
#include <XInput.h>

#define _XM_NO_INTRINSICS
#define XM_NO_ALIGNMENT

#include <xnamath.h>

//Enum used for the state of the current game
enum GameState
{
	eMainMenu,
	eInGame,
	ePauseMenu,
	eEndGame
};

//Main class that manages the game 

class GameManager
{
private:
//instances of the windows handles and windows, used for initialization
	HINSTANCE				m_hInst;
	HWND					m_hWnd;

	int		m_Score;

	Text2D* m_2DText;

	Camera* m_pCamera;
	Camera* m_pThirdPerson;

	//Models for the game
	Model*	  m_pEnemy;
	Model*	  m_pReflectiveCube;
	Model*	  m_pSkybox;
	Model*	  m_pPlayerModel;
	Model*	  m_pPresent;
	Model*	  m_pFloor;
	Model*	  m_pLeftWall;
	Model*	  m_pRightWall;
	Model*	  m_pFrontWall;
	Model*	  m_pBackWall;

	Model*	  m_pObstacle1;
	Model*	  m_pObstacle2;
	Model*	  m_pObstacle3;
	Model*	  m_pObstacle4;

	//SceneNodes for the game
	SceneNode* RootNode;
	SceneNode* m_pEnemyNode;
	SceneNode* m_pReflectiveCubeNode;
	SceneNode* m_pSkyboxNode;
	SceneNode* m_pPlayerNode;
	SceneNode* m_pPresentNode;
	SceneNode* m_pFloorNode;
	SceneNode* m_pLeftWallNode;
	SceneNode* m_pRightWallNode;
	SceneNode* m_pFrontWallNode;
	SceneNode* m_pBackWallNode;
	SceneNode* m_pObstacle1Node;
	SceneNode* m_pObstacle2Node;
	SceneNode* m_pObstacle3Node;
	SceneNode* m_pObstacle4Node;

	SceneNode* cameraNode;

	
	//DirectX structures 
	D3D_DRIVER_TYPE			m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL		m_featureLevel = D3D_FEATURE_LEVEL_11_0; //version of direct x used
	ID3D11Device*			m_pD3DDevice = NULL; //pointer to the D3DDevice
	ID3D11DeviceContext*	m_pImmediateContext = NULL; //Pointer to the Immediate context of the device
	IDXGISwapChain*			m_pSwapChain = NULL;
	ID3D11RenderTargetView*	m_pBackBufferRTView = NULL;

	ID3D11Buffer*			m_pVertexBuffer;
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11InputLayout*		m_pInputLayout;

	ID3D11Buffer*			m_pConstantBuffer0; //Constant buffer pointer

	ID3D11DepthStencilView* m_pZBuffer; //Creates a Z Buffer to allow for correct rendering order

	ID3D11BlendState*		  m_pBlendAlphaEnable; //blend state to turn the alpha channel on
	ID3D11BlendState*		  m_pBlendAlphaDisable; //blend state to turn the alpha channel off

	ID3D11RasterizerState*	  m_pRasterSolid = 0;
	ID3D11RasterizerState*    m_pRasterSkyBox = 0;
	ID3D11DepthStencilState*  m_pDepthWriteSolid = 0;
	ID3D11DepthStencilState*  m_pDepthWriteSkyBox = 0;


public:

	//Enum used for the state of the current game
	GameState m_eGameState;

	MenuSystem* m_pMenu;
	Input*		m_pPlayerInput;

	GameManager();
	~GameManager();

	void CreateLevel();

	HRESULT InitaliseWindow(HINSTANCE hInstance, int nCmdShow); //creates a window and initialize values
	HRESULT InitialiseD3D(); //initialize direct x
	HRESULT InitialiseGraphics(); //set up the scene and related items
	void RenderFrame(); //Render the scene

	void ShutdownD3D(); //cleanup function

	HINSTANCE GetHInstance();
	HWND GetHWnd();

	void GameLogic(); //Game logic loop
	void MainMenu();//main menu loop
	void PauseMenu();//pause menu loop

	

};

