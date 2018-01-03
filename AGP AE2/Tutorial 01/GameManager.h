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

#define _XM_NO_INTRINSICS
#define XM_NO_ALIGNMENT

#include <xnamath.h>

enum GameState
{
	eMainMenu,
	eInGame,
	ePauseMenu,
	eEndGame
};

class GameManager
{
private:

	HINSTANCE				m_hInst;
	HWND					m_hWnd;

	

	int		m_Score;
	bool	m_IsGameRunning;
	bool    m_IsGamePaused;

	Text2D* m_2DText;

	Camera* m_pCamera;
	Camera* m_pThirdPerson;

	Model*	  g_pModel;
	Model*	  g_pModel2;
	Model*	  m_pSkybox;
	Model*	  m_pPlayerModel;
	Model*	  m_pPresent;
	Model*	  m_pFloor;

	SceneNode* RootNode;
	SceneNode* node1;
	SceneNode* node2;
	SceneNode* m_pSkyboxNode;
	SceneNode* m_pPlayerNode;
	SceneNode* m_pPresentNode;
	SceneNode* m_pFloorNode;

	SceneNode* cameraNode;

	

	D3D_DRIVER_TYPE			g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL		g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*			g_pD3DDevice = NULL;
	ID3D11DeviceContext*	g_pImmediateContext = NULL;
	IDXGISwapChain*			g_pSwapChain = NULL;
	ID3D11RenderTargetView*	g_pBackBufferRTView = NULL;

	ID3D11Buffer*			g_pVertexBuffer;
	ID3D11VertexShader*		g_pVertexShader;
	ID3D11PixelShader*		g_pPixelShader;
	ID3D11InputLayout*		g_pInputLayout;

	ID3D11Buffer*			g_pConstantBuffer0; //Constant buffer pointer

	ID3D11DepthStencilView* g_pZBuffer; //Creates a Z Buffer to allow for correct rendering order

	ID3D11BlendState*		  m_pBlendAlphaEnable;
	ID3D11BlendState*		  m_pBlendAlphaDisable;

	ID3D11RasterizerState*	  m_pRasterSolid = 0;
	ID3D11RasterizerState*    m_pRasterSkyBox = 0;
	ID3D11DepthStencilState*  m_pDepthWriteSolid = 0;
	ID3D11DepthStencilState*  m_pDepthWriteSkyBox = 0;


public:

	GameState m_eGameState;

	MenuSystem* m_pMenu;
	Input*					m_pPlayerInput;

	GameManager();
	~GameManager();

	void CreateLevel();

	HRESULT InitaliseWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitialiseD3D();
	HRESULT InitialiseGraphics();
	void RenderFrame();

	void ShutdownD3D();

	HINSTANCE GetHInstance();
	HWND GetHWnd();
	bool GetIsRunning();

	void GameLogic();
	void MainMenu();
	//gameloop
	//playerloop
	//logicloop
	//collisionupdate
	

};

