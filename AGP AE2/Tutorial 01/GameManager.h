#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <stdio.h>
#include "Camera.h"
#include "text2D.h"
#include "Model.h"
#include "Input.h"
#include "SceneNode.h"

#define _XM_NO_INTRINSICS
#define XM_NO_ALIGNMENT

#include <xnamath.h>

class GameManager
{
private:

	HINSTANCE				m_hInst;
	HWND					m_hWnd;

	Text2D* m_2DText;
	Camera* g_camera;

	Model*	  g_pModel;
	Model*	  g_pModel2;
	Model*	  g_pModel3;

	SceneNode* RootNode;
	SceneNode* node1;
	SceneNode* node2;

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

	ID3D11ShaderResourceView* g_pTexture0; // Texture pointer for images to be rendered onto objects
	ID3D11SamplerState*		  g_pSampler0; //Creates a Sampler state for the pixel shader

	ID3D11BlendState*		  m_pBlendAlphaEnable;
	ID3D11BlendState*		  m_pBlendAlphaDisable;

public:

	Input*					m_pPlayerInput;

	GameManager();
	~GameManager();


	HRESULT InitaliseWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitialiseD3D();
	HRESULT InitialiseGraphics();
	void RenderFrame();

	void ShutdownD3D();

	HINSTANCE GetHInstance();
	HWND GetHWnd();

	//renderloop
	//gameloop
	//playerloop
	//logicloop
	//collisionupdate
	

};

