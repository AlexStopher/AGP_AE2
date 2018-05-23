#pragma once



#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include <iostream>
#include <thread>

#include "Camera.h"
#include "text2D.h"
#include "Model.h"
#include "Input.h"
#include "SceneNode.h"
#include "MenuSystem.h"
#include "Math.h"
#include <XInput.h>
#include <mutex>

#include "Sprite.h"

#define _XM_NO_INTRINSICS
#define XM_NO_ALIGNMENT


#include <xnamath.h>

//Main class that manages the game and contains important classes. Can be expanded on with a Level class to keep the scenenode/models clean and readable

//Enum used for the state of the current game
enum GameState
{
	eMainMenu,
	eInGame,
	ePauseMenu,
	eLoading,
	eEndGame

	
};

//Main class that manages the game 

class GameManager
{
private:

	thread Thread1;
	thread Thread2;

	mutex M;
	
//instances of the windows handles and windows, used for initialization
	HINSTANCE				m_hInst;
	HWND					m_hWnd;

	//Temporary loading screen datatypes - to replace once the sprite works
	bool	m_LevelLoaded, m_LevelInitialised;
	int		m_LoadingScreenDots;

	int		m_Score;

	//UI Elements (Build a class for later)
	Text2D* m_p2DText;
	Sprite*	m_pUISprite;
	Sprite*	m_pLoadingScreenIcon;

	//Cameras
	Camera* m_pCamera;
	Camera* m_pThirdPerson;

	//Models for the game
	Model*	  m_pEnemy;
	Model*	  m_pReflectiveCube;
	Model*	  m_pSkybox;
	Model*	  m_pPlayerModel;
	Model*	  m_pPresent;
	Model*	  m_pFloor;

	

#pragma region FrontWall



	Model*	  m_pFrontWall1;
	Model*	  m_pFrontWall2;
	Model*	  m_pFrontWall3;
	Model*	  m_pFrontWall4;
	Model*	  m_pFrontWall5;
	Model*	  m_pFrontWall6;
	Model*	  m_pFrontWall7;
	Model*	  m_pFrontWall8;
	Model*	  m_pFrontWall9;
	Model*	  m_pFrontWall10;
	Model*	  m_pFrontWall11;
	Model*	  m_pFrontWall12;
	Model*	  m_pFrontWall13;

	SceneNode* m_pFrontWallNode1;
	SceneNode* m_pFrontWallNode2;
	SceneNode* m_pFrontWallNode3;
	SceneNode* m_pFrontWallNode4;
	SceneNode* m_pFrontWallNode5;
	SceneNode* m_pFrontWallNode6;
	SceneNode* m_pFrontWallNode7;
	SceneNode* m_pFrontWallNode8;
	SceneNode* m_pFrontWallNode9;
	SceneNode* m_pFrontWallNode10;
	SceneNode* m_pFrontWallNode11;
	SceneNode* m_pFrontWallNode12;
	SceneNode* m_pFrontWallNode13;

#pragma endregion

#pragma region RightWall

	Model*	  m_pRightWall1;
	Model*	  m_pRightWall2;
	Model*	  m_pRightWall3;
	Model*	  m_pRightWall4;
	Model*	  m_pRightWall5;
	Model*	  m_pRightWall6;
	Model*	  m_pRightWall7;
	Model*	  m_pRightWall8;
	Model*	  m_pRightWall9;
	Model*	  m_pRightWall10;
	Model*	  m_pRightWall11;
	Model*	  m_pRightWall12;
	Model*	  m_pRightWall13;

	SceneNode* m_pRightWallNode1;
	SceneNode* m_pRightWallNode2;
	SceneNode* m_pRightWallNode3;
	SceneNode* m_pRightWallNode4;
	SceneNode* m_pRightWallNode5;
	SceneNode* m_pRightWallNode6;
	SceneNode* m_pRightWallNode7;
	SceneNode* m_pRightWallNode8;
	SceneNode* m_pRightWallNode9;
	SceneNode* m_pRightWallNode10;
	SceneNode* m_pRightWallNode11;
	SceneNode* m_pRightWallNode12;
	SceneNode* m_pRightWallNode13;

#pragma endregion

#pragma region LeftWall

	Model*	  m_pLeftWall1;
	Model*	  m_pLeftWall2;
	Model*	  m_pLeftWall3;
	Model*	  m_pLeftWall4;
	Model*	  m_pLeftWall5;
	Model*	  m_pLeftWall6;
	Model*	  m_pLeftWall7;
	Model*	  m_pLeftWall8;
	Model*	  m_pLeftWall9;
	Model*	  m_pLeftWall10;
	Model*	  m_pLeftWall11;
	Model*	  m_pLeftWall12;
	Model*	  m_pLeftWall13;

	SceneNode* m_pLeftWallNode1;
	SceneNode* m_pLeftWallNode2;
	SceneNode* m_pLeftWallNode3;
	SceneNode* m_pLeftWallNode4;
	SceneNode* m_pLeftWallNode5;
	SceneNode* m_pLeftWallNode6;
	SceneNode* m_pLeftWallNode7;
	SceneNode* m_pLeftWallNode8;
	SceneNode* m_pLeftWallNode9;
	SceneNode* m_pLeftWallNode10;
	SceneNode* m_pLeftWallNode11;
	SceneNode* m_pLeftWallNode12;
	SceneNode* m_pLeftWallNode13;

#pragma endregion

#pragma region BackWall

	Model*	  m_pBackWall;
	Model*	  m_pBackWall2;
	Model*	  m_pBackWall3;
	Model*	  m_pBackWall4;
	Model*	  m_pBackWall5;
	Model*	  m_pBackWall6;
	Model*	  m_pBackWall7;
	Model*	  m_pBackWall8;
	Model*	  m_pBackWall9;
	Model*	  m_pBackWall10;
	Model*	  m_pBackWall11;
	Model*	  m_pBackWall12;
	Model*	  m_pBackWall13;

	SceneNode* m_pBackWallNode1;
	SceneNode* m_pBackWallNode2;
	SceneNode* m_pBackWallNode3;
	SceneNode* m_pBackWallNode4;
	SceneNode* m_pBackWallNode5;
	SceneNode* m_pBackWallNode6;
	SceneNode* m_pBackWallNode7;
	SceneNode* m_pBackWallNode8;
	SceneNode* m_pBackWallNode9;
	SceneNode* m_pBackWallNode10;
	SceneNode* m_pBackWallNode11;
	SceneNode* m_pBackWallNode12;
	SceneNode* m_pBackWallNode13;


#pragma endregion

	

	Model*	  m_pObstacle1;
	Model*	  m_pObstacle2;
	Model*	  m_pObstacle3;
	Model*	  m_pObstacle4;

	//SceneNodes for the game
	SceneNode* m_pRootNode;
	SceneNode* m_pEnemyNode;
	SceneNode* m_pReflectiveCubeNode;
	SceneNode* m_pSkyboxNode;
	SceneNode* m_pPlayerNode;
	SceneNode* m_pPresentNode;
	SceneNode* m_pFloorNode;


	

	//Future cleanup
	//vector<SceneNode*>	m_pBackWallSceneNodes;


	SceneNode* m_pObstacle1Node;
	SceneNode* m_pObstacle2Node;
	SceneNode* m_pObstacle3Node;
	SceneNode* m_pObstacle4Node;

	SceneNode* m_pCameraNode;

	
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


	//DirectX::AudioEngine*			  m_pAudioEngine;

public:

	//Enum used for the state of the current game
	GameState m_eGameState;

	MenuSystem* m_pMenu;
	
	//Move to player class later
	Input*		m_pPlayerInput;
	float		m_PlayerMoveSpeed = 0.002f;

	GameManager();
	~GameManager();
	void ShutdownD3D(); //cleanup function

	//
	void SetupLevelDatatypes(); //Initialises object pointers
	void CreateLevel(); //Sets the level data into scene nodes

	HRESULT InitaliseWindow(HINSTANCE hInstance, int nCmdShow); //creates a window and initialize values
	HRESULT InitialiseD3D(); //Initialize direct x
	HRESULT InitialiseGraphics(); //Set up the scene and related items
	void RenderFrame(); //Render the scene

	

	HINSTANCE GetHInstance();
	HWND GetHWnd();

	void GameLogic(); //Game logic loop
	void MainMenu(); //Main menu loop
	void PauseMenu(); //Pause menu loop
	void LoadingScreen(); //Loading Screen with animation
	

};

