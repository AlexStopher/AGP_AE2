#include "GameManager.h"



struct POS_COL_TEX_NORM_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};



GameManager::GameManager()
{
	m_pPlayerInput = new Input;
	m_LevelLoaded = false;
	m_LevelInitialised = false;
	m_LoadingScreenDots = 0;
}


GameManager::~GameManager()
{
}

//Cleanup function
void GameManager::ShutdownD3D()
{

	m_pRasterSolid->Release();
	m_pRasterSkyBox->Release();
    m_pDepthWriteSolid->Release();
	m_pDepthWriteSkyBox->Release();
	m_pMenu->~MenuSystem();
	if (m_pCamera) m_pCamera->~Camera();
	if (m_pZBuffer) m_pZBuffer->Release();
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVertexShader) m_pVertexShader->Release();
	if (m_pPixelShader) m_pPixelShader->Release();
	if (m_pConstantBuffer0) m_pConstantBuffer0->Release();

	if (m_pPlayerInput) m_pPlayerInput->~Input();

	if (m_pBackBufferRTView) m_pBackBufferRTView->Release();
	if (m_pSwapChain) m_pSwapChain->Release();
	if (m_pImmediateContext) m_pImmediateContext->Release();
	if (m_pD3DDevice) m_pD3DDevice->Release();

}
HINSTANCE GameManager::GetHInstance()
{
	return m_hInst;
}

HWND GameManager::GetHWnd()
{
	return m_hWnd;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	return 0;
}

HRESULT GameManager::InitaliseWindow(HINSTANCE hInstance, int nCmdShow)
{
	char Name[100] = "DUUM";

	//Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//	wcex.hbrBackground = (HBRUSH) ( COLOR_WINDOW + 1);
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	//Create window
	m_hInst = hInstance;
	RECT rc = { 0,0,1280,800 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_hWnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!m_hWnd)
		return E_FAIL;

	ShowWindow(m_hWnd, nCmdShow);

	return S_OK;
}



//Create D3D device and swap chain

HRESULT GameManager::InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags != D3D11_CREATE_DEVICE_DEBUG;
#endif
	
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, 
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	//creates an array of feature levels that can be used, with the first item being the
	//one attempted first
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//The swap chain description
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain,
			&m_pD3DDevice, &m_featureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
			break;

	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = m_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&m_pBackBufferRTView);

	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	//Add UI elements
	m_p2DText = new Text2D("assets/font1.png", m_pD3DDevice, m_pImmediateContext);
	m_pUISprite = new Sprite("assets/UIpng.png", m_pD3DDevice, m_pImmediateContext);
	m_pUISprite->AddSprite("assets/UIpng2.png");

	m_pLoadingScreenIcon = new Sprite("assets/LoadingIcon1.png", m_pD3DDevice, m_pImmediateContext);
	m_pLoadingScreenIcon->AddSprite("assets/LoadingIcon2.png");
	m_pLoadingScreenIcon->AddSprite("assets/LoadingIcon3.png");
	m_pLoadingScreenIcon->AddSprite("assets/LoadingIcon4.png");



	//Creates a Z Buffer texture

	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D *pZBufferTexture;
	hr = m_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	//Creates the actual Z Buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = m_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &m_pZBuffer);
	pZBufferTexture->Release();


	// Set the render target view
	m_pImmediateContext->OMSetRenderTargets(1, &m_pBackBufferRTView, m_pZBuffer);

	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_pImmediateContext->RSSetViewports(1, &viewport);



	//Creation of the Alpha Blend description
	D3D11_BLEND_DESC b;
	ZeroMemory(&b, sizeof(b));

	b.RenderTarget[0].BlendEnable = TRUE;
	b.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	b.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	b.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	b.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	b.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	b.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	b.IndependentBlendEnable = FALSE;
	b.AlphaToCoverageEnable = FALSE;

	m_pD3DDevice->CreateBlendState(&b, &m_pBlendAlphaEnable);

	//Rasteriser for creating the skybox and normal states
	D3D11_RASTERIZER_DESC d;
	ZeroMemory(&d, sizeof(d));
	d.FillMode = D3D11_FILL_SOLID;
	d.CullMode = D3D11_CULL_NONE;
	d.DepthClipEnable = false;
	d.FrontCounterClockwise = false;
	d.MultisampleEnable = false;

	hr = m_pD3DDevice->CreateRasterizerState(&d, &m_pRasterSolid);
	d.CullMode = D3D11_CULL_FRONT;
	hr = m_pD3DDevice->CreateRasterizerState(&d, &m_pRasterSkyBox);

	//Depth Stencil to allow the skybox to not be drawn over objects
	D3D11_DEPTH_STENCIL_DESC ds;
	ZeroMemory(&ds, sizeof(ds));
	ds.StencilEnable = false;
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	

	m_pD3DDevice->CreateDepthStencilState(&ds, &m_pDepthWriteSolid);

	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_pD3DDevice->CreateDepthStencilState(&ds, &m_pDepthWriteSkyBox);



	//Audio stuff

	//ZeroMemory(&m_pAudioEngine, sizeof(m_pAudioEngine));

	//m_pAudioEngine->

	return S_OK;
}

HRESULT GameManager::InitialiseGraphics()
{
	HRESULT hr = S_OK;

	//Creates a menu instance and sets the main menu up
	m_pMenu = new MenuSystem(m_pD3DDevice, m_pImmediateContext);
	m_pMenu->SetupMainMenu();




	XMFLOAT3 vertices[] =
	{
		XMFLOAT3(1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f),
		XMFLOAT3(-1.0f, 1.0f, 0.0f),
		XMFLOAT3(-1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, -1.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.0f)

	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // GPU and CPU
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = m_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);



	if (FAILED(hr))
	{
		return hr;
	}

	//Create constant buffer

	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 208;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer0);

	if (FAILED(hr))
	{
		return hr;
	}

	//Copy the certices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//Lock the buffer to allow writing
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));

	//Unlock of Buffer
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	//Load and compile pixel and vertex shaders
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVertexShader);

	if (FAILED(hr))
	{
		return 0;
	}

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPixelShader);

	if (FAILED(hr))
	{
		return 0;
	}

	m_pImmediateContext->VSSetShader(m_pVertexShader, 0, 0);

	m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);


	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	return S_OK;



}

//Creates the levels items and sets the values relevent for the,
void GameManager::SetupLevelDatatypes()
{
	//This is really inefficient and unreadable, can be cleaned up in the future by reusing / managing vectors properly.
	//Main issue is with scene management collision detection, need to have a range check to store certain objects in an array 
	//eg check the 5 closest objects only

	//Initialization of the games models 
#pragma region Environment Creation



	m_pEnemy = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pEnemy->LoadObjModel("assets/WallX.obj");
	m_pEnemy->LoadShader("model_shaders.hlsl");
	m_pEnemy->AddTexture("assets/UIpng.png");


	m_pReflectiveCube = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pReflectiveCube->LoadObjModel("assets/cube.obj");
	m_pReflectiveCube->LoadShader("reflect_shader.hlsl");
	m_pReflectiveCube->AddTexture("assets/skybox02.dds");

	m_pSkybox = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pSkybox->LoadObjModel("assets/cube.obj");
	m_pSkybox->LoadShader("SkyBox_shader.hlsl");
	m_pSkybox->AddTexture("assets/skybox02.dds");

	m_pPresent = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pPresent->LoadObjModel("assets/cube.obj");
	m_pPresent->LoadShader("model_shaders.hlsl");
	m_pPresent->AddTexture("assets/present.bmp");

	m_pFloor = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFloor->LoadObjModel("assets/wallX.obj");
	m_pFloor->LoadShader("model_shaders.hlsl");
	m_pFloor->AddTexture("assets/RedCarpet.png");


	//Left walls

	m_pLeftWall1 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall1->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall1->LoadShader("model_shaders.hlsl");
	m_pLeftWall1->AddTexture("assets/texture.bmp");

	m_pLeftWall2 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall2->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall2->LoadShader("model_shaders.hlsl");
	m_pLeftWall2->AddTexture("assets/texture.bmp");

	m_pLeftWall3 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall3->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall3->LoadShader("model_shaders.hlsl");
	m_pLeftWall3->AddTexture("assets/texture.bmp");

	m_pLeftWall4 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall4->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall4->LoadShader("model_shaders.hlsl");
	m_pLeftWall4->AddTexture("assets/texture.bmp");

	m_pLeftWall5 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall5->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall5->LoadShader("model_shaders.hlsl");
	m_pLeftWall5->AddTexture("assets/texture.bmp");

	m_pLeftWall6 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall6->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall6->LoadShader("model_shaders.hlsl");
	m_pLeftWall6->AddTexture("assets/texture.bmp");

	m_pLeftWall7 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall7->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall7->LoadShader("model_shaders.hlsl");
	m_pLeftWall7->AddTexture("assets/texture.bmp");

	m_pLeftWall8 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall8->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall8->LoadShader("model_shaders.hlsl");
	m_pLeftWall8->AddTexture("assets/texture.bmp");

	m_pLeftWall9 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall9->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall9->LoadShader("model_shaders.hlsl");
	m_pLeftWall9->AddTexture("assets/texture.bmp");

	m_pLeftWall10 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall10->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall10->LoadShader("model_shaders.hlsl");
	m_pLeftWall10->AddTexture("assets/texture.bmp");

	m_pLeftWall11 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall11->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall11->LoadShader("model_shaders.hlsl");
	m_pLeftWall11->AddTexture("assets/texture.bmp");

	m_pLeftWall12 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall12->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall12->LoadShader("model_shaders.hlsl");
	m_pLeftWall12->AddTexture("assets/texture.bmp");

	m_pLeftWall13 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pLeftWall13->LoadObjModel("assets/wallZ.obj");
	m_pLeftWall13->LoadShader("model_shaders.hlsl");
	m_pLeftWall13->AddTexture("assets/texture.bmp");

	//Right wall

	m_pRightWall1 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall1->LoadObjModel("assets/wallZ.obj");
	m_pRightWall1->LoadShader("model_shaders.hlsl");
	m_pRightWall1->AddTexture("assets/texture.bmp");

	m_pRightWall2 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall2->LoadObjModel("assets/wallZ.obj");
	m_pRightWall2->LoadShader("model_shaders.hlsl");
	m_pRightWall2->AddTexture("assets/texture.bmp");

	m_pRightWall3= new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall3->LoadObjModel("assets/wallZ.obj");
	m_pRightWall3->LoadShader("model_shaders.hlsl");
	m_pRightWall3->AddTexture("assets/texture.bmp");

	m_pRightWall4 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall4->LoadObjModel("assets/wallZ.obj");
	m_pRightWall4->LoadShader("model_shaders.hlsl");
	m_pRightWall4->AddTexture("assets/texture.bmp");

	m_pRightWall5 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall5->LoadObjModel("assets/wallZ.obj");
	m_pRightWall5->LoadShader("model_shaders.hlsl");
	m_pRightWall5->AddTexture("assets/texture.bmp");

	m_pRightWall6 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall6->LoadObjModel("assets/wallZ.obj");
	m_pRightWall6->LoadShader("model_shaders.hlsl");
	m_pRightWall6->AddTexture("assets/texture.bmp");

	m_pRightWall7 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall7->LoadObjModel("assets/wallZ.obj");
	m_pRightWall7->LoadShader("model_shaders.hlsl");
	m_pRightWall7->AddTexture("assets/texture.bmp");

	m_pRightWall8 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall8->LoadObjModel("assets/wallZ.obj");
	m_pRightWall8->LoadShader("model_shaders.hlsl");
	m_pRightWall8->AddTexture("assets/texture.bmp");

	m_pRightWall9 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall9->LoadObjModel("assets/wallZ.obj");
	m_pRightWall9->LoadShader("model_shaders.hlsl");
	m_pRightWall9->AddTexture("assets/texture.bmp");

	m_pRightWall10 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall10->LoadObjModel("assets/wallZ.obj");
	m_pRightWall10->LoadShader("model_shaders.hlsl");
	m_pRightWall10->AddTexture("assets/texture.bmp");

	m_pRightWall11 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall11->LoadObjModel("assets/wallZ.obj");
	m_pRightWall11->LoadShader("model_shaders.hlsl");
	m_pRightWall11->AddTexture("assets/texture.bmp");

	m_pRightWall12 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall12->LoadObjModel("assets/wallZ.obj");
	m_pRightWall12->LoadShader("model_shaders.hlsl");
	m_pRightWall12->AddTexture("assets/texture.bmp");

	m_pRightWall13 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pRightWall13->LoadObjModel("assets/wallZ.obj");
	m_pRightWall13->LoadShader("model_shaders.hlsl");
	m_pRightWall13->AddTexture("assets/texture.bmp");

	//front wall
	m_pFrontWall1 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall1->LoadObjModel("assets/wallCorner.obj");
	m_pFrontWall1->LoadShader("model_shaders.hlsl");
	m_pFrontWall1->AddTexture("assets/texture.bmp");

	m_pFrontWall2 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall2->LoadObjModel("assets/wallX.obj");
	m_pFrontWall2->LoadShader("model_shaders.hlsl");
	m_pFrontWall2->AddTexture("assets/texture.bmp");

	m_pFrontWall3 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall3->LoadObjModel("assets/wallX.obj");
	m_pFrontWall3->LoadShader("model_shaders.hlsl");
	m_pFrontWall3->AddTexture("assets/texture.bmp");

	m_pFrontWall4 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall4->LoadObjModel("assets/wallX.obj");
	m_pFrontWall4->LoadShader("model_shaders.hlsl");
	m_pFrontWall4->AddTexture("assets/texture.bmp");

	m_pFrontWall5 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall5->LoadObjModel("assets/wallX.obj");
	m_pFrontWall5->LoadShader("model_shaders.hlsl");
	m_pFrontWall5->AddTexture("assets/texture.bmp");

	m_pFrontWall6 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall6->LoadObjModel("assets/wallX.obj");
	m_pFrontWall6->LoadShader("model_shaders.hlsl");
	m_pFrontWall6->AddTexture("assets/texture.bmp");

	m_pFrontWall7 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall7->LoadObjModel("assets/wallX.obj");
	m_pFrontWall7->LoadShader("model_shaders.hlsl");
	m_pFrontWall7->AddTexture("assets/texture.bmp");

	m_pFrontWall8 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall8->LoadObjModel("assets/wallX.obj");
	m_pFrontWall8->LoadShader("model_shaders.hlsl");
	m_pFrontWall8->AddTexture("assets/texture.bmp");

	m_pFrontWall9 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall9->LoadObjModel("assets/wallX.obj");
	m_pFrontWall9->LoadShader("model_shaders.hlsl");
	m_pFrontWall9->AddTexture("assets/texture.bmp");

	m_pFrontWall10 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall10->LoadObjModel("assets/wallX.obj");
	m_pFrontWall10->LoadShader("model_shaders.hlsl");
	m_pFrontWall10->AddTexture("assets/texture.bmp");

	m_pFrontWall11 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall11->LoadObjModel("assets/wallX.obj");
	m_pFrontWall11->LoadShader("model_shaders.hlsl");
	m_pFrontWall11->AddTexture("assets/texture.bmp");

	m_pFrontWall12 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall12->LoadObjModel("assets/wallX.obj");
	m_pFrontWall12->LoadShader("model_shaders.hlsl");
	m_pFrontWall12->AddTexture("assets/texture.bmp");

	m_pFrontWall13 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pFrontWall13->LoadObjModel("assets/wallCorner.obj");
	m_pFrontWall13->LoadShader("model_shaders.hlsl");
	m_pFrontWall13->AddTexture("assets/texture.bmp");


	//Back wall

	m_pBackWall = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall->LoadObjModel("assets/wallX.obj");
	m_pBackWall->LoadShader("model_shaders.hlsl");
	m_pBackWall->AddTexture("assets/texture.bmp");

	m_pBackWall2 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall2->LoadObjModel("assets/wallX.obj");
	m_pBackWall2->LoadShader("model_shaders.hlsl");
	m_pBackWall2->AddTexture("assets/texture.bmp");

	m_pBackWall3 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall3->LoadObjModel("assets/wallX.obj");
	m_pBackWall3->LoadShader("model_shaders.hlsl");
	m_pBackWall3->AddTexture("assets/texture.bmp");

	m_pBackWall4 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall4->LoadObjModel("assets/wallX.obj");
	m_pBackWall4->LoadShader("model_shaders.hlsl");
	m_pBackWall4->AddTexture("assets/texture.bmp");

	m_pBackWall5 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall5->LoadObjModel("assets/wallX.obj");
	m_pBackWall5->LoadShader("model_shaders.hlsl");
	m_pBackWall5->AddTexture("assets/texture.bmp");

	m_pBackWall6 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall6->LoadObjModel("assets/wallX.obj");
	m_pBackWall6->LoadShader("model_shaders.hlsl");
	m_pBackWall6->AddTexture("assets/texture.bmp");

	m_pBackWall7 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall7->LoadObjModel("assets/wallX.obj");
	m_pBackWall7->LoadShader("model_shaders.hlsl");
	m_pBackWall7->AddTexture("assets/texture.bmp");

	m_pBackWall8 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall8->LoadObjModel("assets/wallX.obj");
	m_pBackWall8->LoadShader("model_shaders.hlsl");
	m_pBackWall8->AddTexture("assets/texture.bmp");

	m_pBackWall9 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall9->LoadObjModel("assets/wallX.obj");
	m_pBackWall9->LoadShader("model_shaders.hlsl");
	m_pBackWall9->AddTexture("assets/texture.bmp");

	m_pBackWall10 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall10->LoadObjModel("assets/wallX.obj");
	m_pBackWall10->LoadShader("model_shaders.hlsl");
	m_pBackWall10->AddTexture("assets/texture.bmp");

	m_pBackWall11 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall11->LoadObjModel("assets/wallCorner.obj");
	m_pBackWall11->LoadShader("model_shaders.hlsl");
	m_pBackWall11->AddTexture("assets/texture.bmp");

	m_pBackWall12 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall12->LoadObjModel("assets/wallX.obj");
	m_pBackWall12->LoadShader("model_shaders.hlsl");
	m_pBackWall12->AddTexture("assets/texture.bmp");

	m_pBackWall13 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pBackWall13->LoadObjModel("assets/wallCorner.obj");
	m_pBackWall13->LoadShader("model_shaders.hlsl");
	m_pBackWall13->AddTexture("assets/texture.bmp");



	m_pObstacle1 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pObstacle1->LoadObjModel("assets/cube.obj");
	m_pObstacle1->LoadShader("model_shaders.hlsl");
	m_pObstacle1->AddTexture("assets/texture.bmp");

	m_pObstacle2 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pObstacle2->LoadObjModel("assets/cube.obj");
	m_pObstacle2->LoadShader("model_shaders.hlsl");
	m_pObstacle2->AddTexture("assets/texture.bmp");

	m_pObstacle3 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pObstacle3->LoadObjModel("assets/cube.obj");
	m_pObstacle3->LoadShader("model_shaders.hlsl");
	m_pObstacle3->AddTexture("assets/texture.bmp");

	m_pObstacle4 = new Model(m_pD3DDevice, m_pImmediateContext);
	m_pObstacle4->LoadObjModel("assets/cube.obj");
	m_pObstacle4->LoadShader("model_shaders.hlsl");
	m_pObstacle4->AddTexture("assets/texture.bmp");



	//Initialization of the games SceneNodes
	m_pRootNode = new SceneNode();
	m_pEnemyNode = new SceneNode();
	m_pReflectiveCubeNode = new SceneNode();
	m_pSkyboxNode = new SceneNode();
	m_pCameraNode = new SceneNode();
	m_pPresentNode = new SceneNode();
	m_pFloorNode = new SceneNode(); 

	m_pLeftWallNode1 = new SceneNode();
	m_pLeftWallNode2 = new SceneNode();
	m_pLeftWallNode3 = new SceneNode();
	m_pLeftWallNode4 = new SceneNode();
	m_pLeftWallNode5 = new SceneNode();
	m_pLeftWallNode6 = new SceneNode();
	m_pLeftWallNode7 = new SceneNode();
	m_pLeftWallNode8 = new SceneNode();
	m_pLeftWallNode9 = new SceneNode();
	m_pLeftWallNode10 = new SceneNode();
	m_pLeftWallNode11 = new SceneNode();
	m_pLeftWallNode12 = new SceneNode();
	m_pLeftWallNode13 = new SceneNode();

	m_pRightWallNode1 = new SceneNode();
	m_pRightWallNode2 = new SceneNode();
	m_pRightWallNode3 = new SceneNode();
	m_pRightWallNode4 = new SceneNode();
	m_pRightWallNode5 = new SceneNode();
	m_pRightWallNode6 = new SceneNode();
	m_pRightWallNode7 = new SceneNode();
	m_pRightWallNode8 = new SceneNode();
	m_pRightWallNode9 = new SceneNode();
	m_pRightWallNode10 = new SceneNode();
	m_pRightWallNode11 = new SceneNode();
	m_pRightWallNode12 = new SceneNode();
	m_pRightWallNode13 = new SceneNode();

	m_pFrontWallNode1 = new SceneNode();
	m_pFrontWallNode2 = new SceneNode();
	m_pFrontWallNode3 = new SceneNode();
	m_pFrontWallNode4 = new SceneNode();
	m_pFrontWallNode5 = new SceneNode();
	m_pFrontWallNode6 = new SceneNode();
	m_pFrontWallNode7 = new SceneNode();
	m_pFrontWallNode8 = new SceneNode();
	m_pFrontWallNode9 = new SceneNode();
	m_pFrontWallNode10 = new SceneNode();
	m_pFrontWallNode11 = new SceneNode();
	m_pFrontWallNode12 = new SceneNode();
	m_pFrontWallNode13 = new SceneNode();

    m_pBackWallNode1 = new SceneNode();
	m_pBackWallNode2 = new SceneNode();
	m_pBackWallNode3 = new SceneNode();
	m_pBackWallNode4 = new SceneNode();
	m_pBackWallNode5 = new SceneNode();
	m_pBackWallNode6 = new SceneNode();
	m_pBackWallNode7 = new SceneNode();
	m_pBackWallNode8 = new SceneNode();
	m_pBackWallNode9 = new SceneNode();
	m_pBackWallNode10 = new SceneNode();
	m_pBackWallNode11 = new SceneNode();
	m_pBackWallNode12 = new SceneNode();
	m_pBackWallNode13 = new SceneNode();

	m_pObstacle1Node = new SceneNode();
	m_pObstacle2Node = new SceneNode();
	m_pObstacle3Node = new SceneNode();
	m_pObstacle4Node = new SceneNode();

#pragma endregion

	m_pCamera = new Camera(0.0f, 0.0f, 0.0, 0.0f);
	m_pThirdPerson = new Camera(-3.0f, 1.0f, 0.0f, 0.0f);

	m_LevelInitialised = true;
}

void GameManager::CreateLevel()
{

	//Adding all of the relevent Nodes to the RootNode for Scene management
	m_pRootNode->AddChildNode(m_pEnemyNode);
	m_pRootNode->AddChildNode(m_pReflectiveCubeNode);
	m_pRootNode->AddChildNode(m_pPresentNode);
	m_pRootNode->AddChildNode(m_pFloorNode);

	m_pRootNode->AddChildNode(m_pRightWallNode1);
	m_pRootNode->AddChildNode(m_pRightWallNode2);
	m_pRootNode->AddChildNode(m_pRightWallNode3);
	m_pRootNode->AddChildNode(m_pRightWallNode4);
	m_pRootNode->AddChildNode(m_pRightWallNode5);
	m_pRootNode->AddChildNode(m_pRightWallNode6);
	m_pRootNode->AddChildNode(m_pRightWallNode7);
	m_pRootNode->AddChildNode(m_pRightWallNode8);
	m_pRootNode->AddChildNode(m_pRightWallNode9);
	m_pRootNode->AddChildNode(m_pRightWallNode10);
	m_pRootNode->AddChildNode(m_pRightWallNode11);
	m_pRootNode->AddChildNode(m_pRightWallNode12);
	m_pRootNode->AddChildNode(m_pRightWallNode13);

	m_pRootNode->AddChildNode(m_pLeftWallNode1);
	m_pRootNode->AddChildNode(m_pLeftWallNode2);
	m_pRootNode->AddChildNode(m_pLeftWallNode3);
	m_pRootNode->AddChildNode(m_pLeftWallNode4);
	m_pRootNode->AddChildNode(m_pLeftWallNode5);
	m_pRootNode->AddChildNode(m_pLeftWallNode6);
	m_pRootNode->AddChildNode(m_pLeftWallNode7);
	m_pRootNode->AddChildNode(m_pLeftWallNode8);
	m_pRootNode->AddChildNode(m_pLeftWallNode9);
	m_pRootNode->AddChildNode(m_pLeftWallNode10);
	m_pRootNode->AddChildNode(m_pLeftWallNode11);
	m_pRootNode->AddChildNode(m_pLeftWallNode12);
	m_pRootNode->AddChildNode(m_pLeftWallNode13);


	m_pRootNode->AddChildNode(m_pFrontWallNode1);
	m_pRootNode->AddChildNode(m_pFrontWallNode2);
	m_pRootNode->AddChildNode(m_pFrontWallNode3);
	m_pRootNode->AddChildNode(m_pFrontWallNode4);
	m_pRootNode->AddChildNode(m_pFrontWallNode5);
	m_pRootNode->AddChildNode(m_pFrontWallNode6);
	m_pRootNode->AddChildNode(m_pFrontWallNode7);
	m_pRootNode->AddChildNode(m_pFrontWallNode8);
	m_pRootNode->AddChildNode(m_pFrontWallNode9);
	m_pRootNode->AddChildNode(m_pFrontWallNode10);
	m_pRootNode->AddChildNode(m_pFrontWallNode11);
	m_pRootNode->AddChildNode(m_pFrontWallNode12);
	m_pRootNode->AddChildNode(m_pFrontWallNode13);

	m_pRootNode->AddChildNode(m_pBackWallNode1);
	m_pRootNode->AddChildNode(m_pBackWallNode2);
	m_pRootNode->AddChildNode(m_pBackWallNode3);
	m_pRootNode->AddChildNode(m_pBackWallNode4);
	m_pRootNode->AddChildNode(m_pBackWallNode5);
	m_pRootNode->AddChildNode(m_pBackWallNode6);
	m_pRootNode->AddChildNode(m_pBackWallNode7);
	m_pRootNode->AddChildNode(m_pBackWallNode8);
	m_pRootNode->AddChildNode(m_pBackWallNode9);
	m_pRootNode->AddChildNode(m_pBackWallNode10);
	m_pRootNode->AddChildNode(m_pBackWallNode11);
	m_pRootNode->AddChildNode(m_pBackWallNode12);
	m_pRootNode->AddChildNode(m_pBackWallNode13);

	m_pRootNode->AddChildNode(m_pObstacle1Node);
	m_pRootNode->AddChildNode(m_pObstacle2Node);
	m_pRootNode->AddChildNode(m_pObstacle3Node);
	m_pRootNode->AddChildNode(m_pObstacle4Node);

	//Wall/floor code
	m_pFloorNode->AddModel(m_pFloor);
	m_pFloorNode->SetRotationX(-90, m_pRootNode);
	m_pFloorNode->SetYPos(-51, m_pRootNode);
	m_pFloorNode->SetScale(50, m_pRootNode);
	m_pFloorNode->SetCanObjectCollide(false);

	//Front wall

	m_pFrontWallNode1->AddModel(m_pFrontWall1);
	m_pFrontWallNode1->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode1->SetXPos(-14.0f, m_pRootNode);

	m_pFrontWallNode13->AddModel(m_pFrontWall13);
	m_pFrontWallNode13->SetRotationY(90, m_pRootNode);
	m_pFrontWallNode13->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode13->SetXPos(10.0f, m_pRootNode);

	m_pFrontWallNode2->AddModel(m_pFrontWall2);
	m_pFrontWallNode2->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode2->SetXPos(-12.0f, m_pRootNode);

	m_pFrontWallNode3->AddModel(m_pFrontWall3);
	m_pFrontWallNode3->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode3->SetXPos(-10.0f, m_pRootNode);

	m_pFrontWallNode4->AddModel(m_pFrontWall4);
	m_pFrontWallNode4->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode4->SetXPos(-8.0f, m_pRootNode);

	m_pFrontWallNode5->AddModel(m_pFrontWall5);
	m_pFrontWallNode5->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode5->SetXPos(-6.0f, m_pRootNode);

	m_pFrontWallNode6->AddModel(m_pFrontWall6);
	m_pFrontWallNode6->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode6->SetXPos(-4.0f, m_pRootNode);

	m_pFrontWallNode7->AddModel(m_pFrontWall7);
	m_pFrontWallNode7->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode7->SetXPos(-2.0f, m_pRootNode);

	m_pFrontWallNode8->AddModel(m_pFrontWall8);
	m_pFrontWallNode8->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode8->SetXPos(0.0f, m_pRootNode);

	m_pFrontWallNode9->AddModel(m_pFrontWall9);
	m_pFrontWallNode9->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode9->SetXPos(2.0f, m_pRootNode);

	m_pFrontWallNode10->AddModel(m_pFrontWall10);
	m_pFrontWallNode10->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode10->SetXPos(4.0f, m_pRootNode);

	m_pFrontWallNode11->AddModel(m_pFrontWall11);
	m_pFrontWallNode11->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode11->SetXPos(6.0f, m_pRootNode);

	m_pFrontWallNode12->AddModel(m_pFrontWall12);
	m_pFrontWallNode12->SetZPos(20.0f, m_pRootNode);
	m_pFrontWallNode12->SetXPos(8.0f, m_pRootNode);



	m_pBackWallNode1->AddModel(m_pBackWall);;
	m_pBackWallNode1->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode1->SetXPos(-10.0f, m_pRootNode);

	m_pBackWallNode2->AddModel(m_pBackWall2);
	m_pBackWallNode2->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode2->SetXPos(-8.0f, m_pRootNode);

	m_pBackWallNode3->AddModel(m_pBackWall3);
	m_pBackWallNode3->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode3->SetXPos(-6.0f, m_pRootNode);

	m_pBackWallNode4->AddModel(m_pBackWall4);
	m_pBackWallNode4->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode4->SetXPos(-4.0f, m_pRootNode);

	m_pBackWallNode5->AddModel(m_pBackWall5);
	m_pBackWallNode5->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode5->SetXPos(-2.0f, m_pRootNode);

	m_pBackWallNode6->AddModel(m_pBackWall6);
	m_pBackWallNode6->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode6->SetXPos(0.0f, m_pRootNode);

	m_pBackWallNode7->AddModel(m_pBackWall7);
	m_pBackWallNode7->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode7->SetXPos(2.0f, m_pRootNode);

	m_pBackWallNode8->AddModel(m_pBackWall8);
	m_pBackWallNode8->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode8->SetXPos(4.0f, m_pRootNode);

	m_pBackWallNode9->AddModel(m_pBackWall9);
	m_pBackWallNode9->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode9->SetXPos(6.0f, m_pRootNode);

	m_pBackWallNode10->AddModel(m_pBackWall10);
	m_pBackWallNode10->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode10->SetXPos(8.0f, m_pRootNode);

	m_pBackWallNode12->AddModel(m_pBackWall12);
	m_pBackWallNode12->SetZPos(-10.0f, m_pRootNode);
	m_pBackWallNode12->SetXPos(-12.0f, m_pRootNode);

	//Bottom right corner
	m_pBackWallNode11->AddModel(m_pBackWall11);
	m_pBackWallNode11->SetRotationY(-180, m_pRootNode);
	m_pBackWallNode11->SetZPos(-8.0f, m_pRootNode);
	m_pBackWallNode11->SetXPos(10.0f, m_pRootNode);


	//Bottom left corner
	m_pBackWallNode13->AddModel(m_pBackWall13);
	m_pBackWallNode13->SetRotationY(-90, m_pRootNode);
	m_pBackWallNode13->SetZPos(-8.0f, m_pRootNode);
	m_pBackWallNode13->SetXPos(-14.0f, m_pRootNode);


	//Right wall
	m_pRightWallNode1->AddModel(m_pRightWall1);
	m_pRightWallNode1->SetZPos(-6.0f, m_pRootNode);
	m_pRightWallNode1->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode2->AddModel(m_pRightWall2);
	m_pRightWallNode2->SetZPos(-4.0f, m_pRootNode);
	m_pRightWallNode2->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode3->AddModel(m_pRightWall3);
	m_pRightWallNode3->SetZPos(-2.0f, m_pRootNode);
	m_pRightWallNode3->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode4->AddModel(m_pRightWall4);
	m_pRightWallNode4->SetZPos(0.0f, m_pRootNode);
	m_pRightWallNode4->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode5->AddModel(m_pRightWall5);
	m_pRightWallNode5->SetZPos(2.0f, m_pRootNode);
	m_pRightWallNode5->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode6->AddModel(m_pRightWall6);
	m_pRightWallNode6->SetZPos(4.0f, m_pRootNode);
	m_pRightWallNode6->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode7->AddModel(m_pRightWall7);
	m_pRightWallNode7->SetZPos(6.0f, m_pRootNode);
	m_pRightWallNode7->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode8->AddModel(m_pRightWall8);
	m_pRightWallNode8->SetZPos(8.0f, m_pRootNode);
	m_pRightWallNode8->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode9->AddModel(m_pRightWall9);
	m_pRightWallNode9->SetZPos(10.0f, m_pRootNode);
	m_pRightWallNode9->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode10->AddModel(m_pRightWall10);
	m_pRightWallNode10->SetZPos(12.0f, m_pRootNode);
	m_pRightWallNode10->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode11->AddModel(m_pRightWall11);
	m_pRightWallNode11->SetZPos(14.0f, m_pRootNode);
	m_pRightWallNode11->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode12->AddModel(m_pRightWall12);
	m_pRightWallNode12->SetZPos(16.0f, m_pRootNode);
	m_pRightWallNode12->SetXPos(11.0f, m_pRootNode);

	m_pRightWallNode13->AddModel(m_pRightWall13);
	m_pRightWallNode13->SetZPos(18.0f, m_pRootNode);
	m_pRightWallNode13->SetXPos(11.0f, m_pRootNode);

	//Left wall

	m_pLeftWallNode1->AddModel(m_pLeftWall1);
	m_pLeftWallNode1->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode1->SetZPos(-6.0f, m_pRootNode);
	m_pLeftWallNode1->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode2->AddModel(m_pLeftWall2);
	m_pLeftWallNode2->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode2->SetZPos(-4.0f, m_pRootNode);
	m_pLeftWallNode2->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode3->AddModel(m_pLeftWall3);
	m_pLeftWallNode3->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode3->SetZPos(-2.0f, m_pRootNode);
	m_pLeftWallNode3->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode4->AddModel(m_pLeftWall4);
	m_pLeftWallNode4->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode4->SetZPos(0.0f, m_pRootNode);
	m_pLeftWallNode4->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode5->AddModel(m_pLeftWall5);
	m_pLeftWallNode5->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode5->SetZPos(2.0f, m_pRootNode);
	m_pLeftWallNode5->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode6->AddModel(m_pLeftWall6);
	m_pLeftWallNode6->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode6->SetZPos(4.0f, m_pRootNode);
	m_pLeftWallNode6->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode7->AddModel(m_pLeftWall7);
	m_pLeftWallNode7->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode7->SetZPos(6.0f, m_pRootNode);
	m_pLeftWallNode7->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode8->AddModel(m_pLeftWall8);
	m_pLeftWallNode8->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode8->SetZPos(8.0f, m_pRootNode);
	m_pLeftWallNode8->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode9->AddModel(m_pLeftWall9);
	m_pLeftWallNode9->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode9->SetZPos(10.0f, m_pRootNode);
	m_pLeftWallNode9->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode10->AddModel(m_pLeftWall10);
	m_pLeftWallNode10->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode10->SetZPos(12.0f, m_pRootNode);
	m_pLeftWallNode10->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode11->AddModel(m_pLeftWall11);
	m_pLeftWallNode11->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode11->SetZPos(14.0f, m_pRootNode);
	m_pLeftWallNode11->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode12->AddModel(m_pLeftWall12);
	m_pLeftWallNode12->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode12->SetZPos(16.0f, m_pRootNode);
	m_pLeftWallNode12->SetXPos(-15.0f, m_pRootNode);

	m_pLeftWallNode13->AddModel(m_pLeftWall13);
	m_pLeftWallNode13->SetRotationY(180, m_pRootNode);
	m_pLeftWallNode13->SetZPos(18.0f, m_pRootNode);
	m_pLeftWallNode13->SetXPos(-15.0f, m_pRootNode);

	//Interactables
	m_pEnemyNode->AddModel(m_pEnemy);
	m_pReflectiveCubeNode->AddModel(m_pReflectiveCube);
	m_pPresentNode->AddModel(m_pPresent);

	m_pEnemyNode->SetZPos(10.0f, m_pRootNode);
	m_pReflectiveCubeNode->SetZPos(10.0f, m_pRootNode);
	m_pReflectiveCubeNode->SetXPos(0.0f, m_pRootNode);
	m_pReflectiveCubeNode->SetYPos(4.0f, m_pRootNode);


	m_pPresentNode->SetZPos(5.0f, m_pRootNode);
	m_pPresentNode->SetXPos(5.0f, m_pRootNode);
	m_pPresentNode->SetScale(0.6f, m_pRootNode);


	m_pSkyboxNode->AddModel(m_pSkybox);
	m_pSkyboxNode->SetCanObjectCollide(false);
	m_pSkyboxNode->SetScale(4.0f, m_pRootNode);

	//Static Objects
	m_pObstacle1Node->AddModel(m_pObstacle1);
	m_pObstacle1Node->SetXPos(-10, m_pRootNode);
	m_pObstacle1Node->SetZPos(10, m_pRootNode);
	m_pObstacle1Node->SetRotationY(45, m_pRootNode);

	m_pObstacle2Node->AddModel(m_pObstacle1);
	m_pObstacle2Node->SetXPos(-10, m_pRootNode);
	m_pObstacle2Node->SetZPos(-100, m_pRootNode);
	m_pObstacle2Node->SetRotationY(45, m_pRootNode);

	m_pObstacle3Node->AddModel(m_pObstacle1);
	m_pObstacle3Node->SetXPos(10, m_pRootNode);
	m_pObstacle3Node->SetZPos(1000, m_pRootNode);
	m_pObstacle3Node->SetRotationY(45, m_pRootNode);

	m_pObstacle4Node->AddModel(m_pObstacle1);
	m_pObstacle4Node->SetXPos(10, m_pRootNode);
	m_pObstacle4Node->SetZPos(-1000, m_pRootNode);
	m_pObstacle4Node->SetRotationY(45, m_pRootNode);

	m_LevelLoaded = true;
}


void GameManager::RenderFrame(void)
{

	float rgba_clear_colour[4] = { 0.1f, 0.2f,0.6f, 1.0f };
	//Clear the back buffer
	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTView, rgba_clear_colour);

	//clear the Z Buffer
	m_pImmediateContext->ClearDepthStencilView(m_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Render here
	m_p2DText->AddText("Score" + std::to_string(m_Score), -1.0f, +1.0f, 0.1f);



	//Select primitive type
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world, projection, view;

	world = XMMatrixIdentity();

	//Matrix that represents the field of view
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 640.0f / 480.0f, 0.01f, 100.0f);

	//Camera view point
	view = m_pCamera->GetViewMatrix();

	//Lighting for the world and objects
	m_pEnemy->SetDirectionalLight(0.0f, 0.5f, -1.0f, 0.0f);
	m_pReflectiveCube->SetDirectionalLight(0.0f, 0.5f, -1.0f, 0.0f);

	m_pFrontWall1->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall2->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall3->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall4->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall5->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall6->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall7->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall8->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall9->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall10->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall11->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall12->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);
	m_pFrontWall13->SetDirectionalLight(0.0f, 0.0f, 1.0f, 0.0f);

	m_pRightWall1->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall2->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall3->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall4->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall5->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall6->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall7->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall8->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall9->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall10->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall11->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall12->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);
	m_pRightWall13->SetDirectionalLight(0.1f, 0.0f, 0.9f, 0.0f);

	m_pLeftWall1->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall2->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall3->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall4->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall5->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall6->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall7->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall8->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall9->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall10->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall11->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall12->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);
	m_pLeftWall13->SetDirectionalLight(-0.1f, 0.0f, 0.9f, 0.0f);

	m_pObstacle1->SetDirectionalLight(0.0f, 0.5f, -1.0f, 0.0f);
	m_pObstacle2->SetDirectionalLight(0.0f, 0.5f, -1.0f, 0.0f);
	m_pObstacle3->SetDirectionalLight(0.0f, 0.5f, -1.0f, 0.0f);
	m_pObstacle4->SetDirectionalLight(0.0f, 0.5f, -1.0f, 0.0f);

	m_pPresent->SetDirectionalLight(0.0f, 0.5f, -1.0f, 0.0f);
	m_pPresent->SetPointLight(0.0f, 10.0f, 0.0f, 0.0f);
	m_pPresent->SetPointLightColour(0.0f, 1.0f, 0.0f, 0.0f);

	m_pFloor->SetDirectionalLight(0.0f, 0.6f, -1.0f, 0.0f);

	//Sets the raster state and depth stencil for the skybox before setting the default states
	m_pImmediateContext->RSSetState(m_pRasterSkyBox);	
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSkyBox, 0);

	m_pSkyboxNode->Execute(&world, &view, &projection);

	m_pImmediateContext->OMSetDepthStencilState(m_pDepthWriteSolid, 0);
	m_pImmediateContext->RSSetState(m_pRasterSolid);

	//Draw all of the nodes models
	m_pRootNode->Execute(&world, &view, &projection);

	//Renders text after enabling the alpha channel
	m_pImmediateContext->OMSetBlendState(m_pBlendAlphaEnable, 0, 0xffffffff);
	m_p2DText->RenderText();
	
	m_pImmediateContext->OMSetBlendState(m_pBlendAlphaDisable, 0, 0xffffffff);

	//m_pLoadingScreenIcon->Draw();

	m_pSwapChain->Present(0, 0);
}

void GameManager::GameLogic()
{


	//Reads players input
	m_pPlayerInput->ReadInputStates();

	XMMATRIX identity = XMMatrixIdentity();
	m_pRootNode->UpdateCollisionTree(&identity, 1.0f);

	if (m_pPlayerInput->IsKeyPressed(DIK_A))
	{
		m_pCamera->Strafe(0.002f);

		xyz Lookat = m_pCamera->GetCameraRight();

		Lookat.x *= 0.1f;
		Lookat.y *= 0.1f;
		Lookat.z *= 0.1f;


		if (m_pPresentNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
		{
			m_Score += 100;
			m_pPresentNode->SetXPos(Math::GetRandomNumber(10, -10), m_pRootNode);
			m_pPresentNode->SetZPos(Math::GetRandomNumber(10, -10), m_pRootNode);
		}

		if (m_pRootNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, true) == true)
		{
			m_pCamera->Strafe(-0.002f);

		}
	
		
	}
	else if (m_pPlayerInput->IsKeyPressed(DIK_D))
	{
		m_pCamera->Strafe(-0.002f);

		xyz Lookat = m_pCamera->GetCameraRight();

		Lookat.x *= 0.1f;
		Lookat.y *= 0.1f;
		Lookat.z *= 0.1f;

		if (m_pPresentNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
		{
			m_Score += 100;
			m_pPresentNode->SetXPos(Math::GetRandomNumber(10, -10), m_pRootNode);
			m_pPresentNode->SetZPos(Math::GetRandomNumber(10, -10), m_pRootNode);
		}

		if (m_pRootNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, true) == true)
		{
			m_pCamera->Strafe(0.002f);
			
		}

		
	}


	//Player input code
	//If the player presses W, check collisions and interactions moving forward
	if (m_pPlayerInput->IsKeyPressed(DIK_W))
	{
		m_pCamera->Forward(0.002f);
		m_pThirdPerson->Forward(0.002f);

		xyz Lookat = m_pCamera->GetLookAt();

		Lookat.x *= 0.1f;
		Lookat.y *= 0.1f;
		Lookat.z *= 0.1f;

		//Checks for a collision with an object and reverses the movement if so
		if (m_pPresentNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
		{
			m_Score += 100;
			m_pPresentNode->SetXPos(Math::GetRandomNumber(10, -10), m_pRootNode);
			m_pPresentNode->SetZPos(Math::GetRandomNumber(10, -10), m_pRootNode);
		}
		//Checks for collision with the present and if so moves the present to a new position
		if (m_pRootNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, true) == true)
		{
			m_pThirdPerson->Forward(-0.002f);
			m_pCamera->Forward(-0.002f);
		}

		


	}	
	else if (m_pPlayerInput->IsKeyPressed(DIK_S))
	{
		m_pCamera->Forward(-0.002f);
		m_pThirdPerson->Forward(-0.002f);

		xyz Lookat = m_pCamera->GetLookAt();

		Lookat.x *= 0.1f;
		Lookat.y *= 0.1f;
		Lookat.z *= 0.1f;

		if (m_pPresentNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
		{
			m_Score += 100;
			m_pPresentNode->SetXPos(Math::GetRandomNumber(10, -10), m_pRootNode);
			m_pPresentNode->SetZPos(Math::GetRandomNumber(10, -10), m_pRootNode);
		}
		
		if (m_pRootNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, true) == true)
		{
			m_pCamera->Forward(0.002f);
			m_pThirdPerson->Forward(0.002f);
		}		 	

	}

	if (m_pPlayerInput->IsKeyPressed(DIK_ESCAPE))
		m_eGameState = ePauseMenu;

	//Mouse input code

	if (m_pPlayerInput->HasMouseMoved())
	{
		m_pCamera->Rotate(m_pPlayerInput->GetMouseX() / 10);
		//Rotate Y as well?
	}

	//TODO - Add proper deadzones for the controller analogues
#pragma region ControllerInput

	


	if (m_pPlayerInput->IsButtonPressed(XINPUT_GAMEPAD_START))
		m_eGameState = ePauseMenu;

	if (m_pPlayerInput->GetControllerLeftAnalogueY() >= 10000.0f
		|| m_pPlayerInput->IsButtonPressed(XINPUT_GAMEPAD_DPAD_UP))
	{
		m_pCamera->Forward(0.002f);
		m_pThirdPerson->Forward(0.002f);

		xyz Lookat = m_pCamera->GetLookAt();

		Lookat.x *= 0.008f;
		Lookat.y *= 0.008f;
		Lookat.z *= 0.008f;

		//Checks for a collision with an object and reverses the movement if so
		if (m_pPresentNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
		{
			m_Score += 100;
			m_pPresentNode->SetXPos(Math::GetRandomNumber(10, -10), m_pRootNode);
			m_pPresentNode->SetZPos(Math::GetRandomNumber(10, -10), m_pRootNode);			

			Thread1 = thread([=] { m_pPlayerInput->SetControllerVibration(20000.0f, 20000.0f, 1.0f); });
			Thread1.detach();

		}
		//Checks for collision with the present and if so moves the present to a new position
		if (m_pRootNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, true) == true)
		{
			m_pThirdPerson->Forward(-0.002f);
			m_pCamera->Forward(-0.002f);
		}

	
		
	}
	else if (m_pPlayerInput->GetControllerLeftAnalogueY() <= -10000.0f
		|| m_pPlayerInput->IsButtonPressed(XINPUT_GAMEPAD_DPAD_DOWN))
	{
		m_pCamera->Forward(-0.002f);
		m_pThirdPerson->Forward(-0.002f);

		xyz Lookat = m_pCamera->GetLookAt();

		Lookat.x *= -0.008f;
		Lookat.y *= -0.008f;
		Lookat.z *= -0.008f;

		if (m_pPresentNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
		{
			m_Score += 100;
			m_pPresentNode->SetXPos(Math::GetRandomNumber(10, -10), m_pRootNode);
			m_pPresentNode->SetZPos(Math::GetRandomNumber(10, -10), m_pRootNode);

			Thread1 = thread([=] { m_pPlayerInput->SetControllerVibration(20000.0f, 20000.0f, 1.0f); });
			Thread1.detach();
		}

		if (m_pRootNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, true) == true)
		{
			m_pCamera->Forward(0.002f);
			m_pThirdPerson->Forward(0.002f);
		}

	}

	if (m_pPlayerInput->GetControllerLeftAnalogueX() >= 10000.0f
		|| m_pPlayerInput->IsButtonPressed(XINPUT_GAMEPAD_DPAD_RIGHT))
	{
		m_pCamera->Strafe(-0.002f);

		xyz Lookat = m_pCamera->GetCameraRight();

		Lookat.x *= 0.008f;
		Lookat.y *= 0.008f;
		Lookat.z *= 0.008f;

		if (m_pPresentNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
		{
			m_Score += 100;
			m_pPresentNode->SetXPos(Math::GetRandomNumber(10, -10), m_pRootNode);
			m_pPresentNode->SetZPos(Math::GetRandomNumber(10, -10), m_pRootNode);

			Thread1 = thread([=] { m_pPlayerInput->SetControllerVibration(20000.0f, 20000.0f, 1.0f); });
			Thread1.detach();
		}

		if (m_pRootNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, true) == true)
		{
			m_pCamera->Strafe(0.002f);

		}


	}	
	else if (m_pPlayerInput->GetControllerLeftAnalogueX() <= -10000.0f
		|| m_pPlayerInput->IsButtonPressed(XINPUT_GAMEPAD_DPAD_LEFT))
	{	
		m_pCamera->Strafe(0.002f);

		xyz Lookat = m_pCamera->GetCameraRight();

		Lookat.x *= -0.008f;
		Lookat.y *= -0.008f;
		Lookat.z *= -0.008f;


		if (m_pPresentNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
		{
			m_Score += 100;
			m_pPresentNode->SetXPos(Math::GetRandomNumber(14, -8), m_pRootNode);
			m_pPresentNode->SetZPos(Math::GetRandomNumber(14, -8), m_pRootNode);

			Thread1 = thread([=] { m_pPlayerInput->SetControllerVibration(20000.0f, 20000.0f, 1.0f); });
			Thread1.detach();
		}

		if (m_pRootNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, true) == true)
		{
			m_pCamera->Strafe(-0.002f);

		}
	}

	if (m_pPlayerInput->GetControllerRightAnalogueX() >= 20000.0f)
	{
		m_pCamera->Rotate(0.102f);
	}
	else if (m_pPlayerInput->GetControllerRightAnalogueX() <= -20000.0f)
	{
		m_pCamera->Rotate(-0.102f);
	}

#pragma endregion


	//Debug/Testing for collision

	if (m_pPlayerInput->IsKeyPressed(DIK_K))
		m_pBackWallNode13->IncXPos(0.01f, m_pRootNode);

	if (m_pPlayerInput->IsKeyPressed(DIK_H))
		m_pBackWallNode13->IncXPos(-0.01f, m_pRootNode);

	if (m_pPlayerInput->IsKeyPressed(DIK_U))
		m_pBackWallNode13->IncRotY(0.01f, m_pRootNode);

	if (m_pPlayerInput->IsKeyPressed(DIK_J))
		m_pBackWallNode13->IncRotY(-0.01f, m_pRootNode);

	xyz Lookat = m_pCamera->GetLookAt();

	//Enemy "AI" that follows the player around
	m_pEnemyNode->LookAtXYZ(m_pCamera->GetX(), m_pCamera->GetY(), m_pCamera->GetZ(), m_pRootNode);
	m_pEnemyNode->MoveForward(0.001f, m_pRootNode);


	Lookat.x *= 0.002f;
	Lookat.y *= 0.002f;
	Lookat.z *= 0.002f;

	//ends the game if the enemy catches up with the player or the player earns 1000 points
	if (m_pEnemyNode->CheckRaycastCollision(m_pCamera->GetCameraPos(), Lookat, false) == true)
	{
		m_eGameState = eEndGame;
	}
	else if (m_Score >= 1000)
	{
		m_eGameState = eEndGame;
	}

	xyz PresentPos(m_pPresentNode->GetXPos(), m_pPresentNode->GetYPos(), m_pPresentNode->GetZPos());

	/*if (m_pEnemyNode->CheckRaycastCollision(PresentPos, Lookat, false) == true)
	{
		m_pEnemyNode->MoveForward(0.001f, false, m_pRootNode);
	}*/

	//Skybox code
	m_pSkyboxNode->SetXPos(m_pCamera->GetX(), m_pRootNode);
	m_pSkyboxNode->SetYPos(m_pCamera->GetY(), m_pRootNode);
	m_pSkyboxNode->SetZPos(m_pCamera->GetZ(), m_pRootNode);

	
}

//Main menu loop
void GameManager::MainMenu()
{
	float rgba_clear_colour[4] = { 0.1f, 0.2f,0.6f, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTView, rgba_clear_colour);
	m_pPlayerInput->ReadInputStates();

	m_pMenu->MainMenuLoop(m_pPlayerInput);
	m_pSwapChain->Present(0, 0);
	
	if (m_pMenu->m_ePlayerSelection == eQuit && m_pMenu->GetSelection() == true)
	{
		m_eGameState = eEndGame;
	}
	else if (m_pMenu->m_ePlayerSelection == eStartGame && m_pMenu->GetSelection() == true)
	{
		m_eGameState = eLoading;
		//SetupLevelDatatypes(); CreateLevel();
		
	}

}
//Pause menu loop
void GameManager::PauseMenu()
{
	float rgba_clear_colour[4] = { 0.1f, 0.2f,0.6f, 1.0f };

	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTView, rgba_clear_colour);
	m_pPlayerInput->ReadInputStates();

	m_pMenu->PauseMenu(m_pPlayerInput);
	m_pSwapChain->Present(0, 0);

	if (m_pMenu->m_ePlayerSelection == eQuit && m_pMenu->GetSelection() == true)
	{
		m_eGameState = eEndGame;
	}
	else if (m_pMenu->m_ePlayerSelection == eStartGame && m_pMenu->GetSelection() == true)
	{
		m_eGameState = eInGame;
	}

}

void GameManager::LoadingScreen()
{
	//Placeholder until the loading sprite works properly, add in code to slow down the animation as well
	float rgba_clear_colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	m_pImmediateContext->ClearRenderTargetView(m_pBackBufferRTView, rgba_clear_colour);

	if (m_LoadingScreenDots == 0)
	{
		m_p2DText->AddText("Loading", -1.0f, -0.7f, 0.1f);
		m_LoadingScreenDots++;
	}
	else if (m_LoadingScreenDots == 1)
	{
		m_p2DText->AddText("Loading.", -1.0f, -0.7f, 0.1f);
		m_LoadingScreenDots++;
	}
	else if (m_LoadingScreenDots == 2)
	{
		m_p2DText->AddText("Loading..", -1.0f, -0.7f, 0.1f);
		m_LoadingScreenDots++;
	}
	else if (m_LoadingScreenDots == 3)
	{
		m_p2DText->AddText("Loading...", -1.0f, -0.7f, 0.1f);
		m_LoadingScreenDots = 0;
	}

	m_p2DText->RenderText();

	m_pSwapChain->Present(0, 0);

	if (m_LevelInitialised == true && m_LevelLoaded == true)
		m_eGameState = eInGame;

	//This doesnt work for an actual loading screen, figure out how multithreading works properly for WINAPI
	Thread1 = thread([=] { SetupLevelDatatypes(); CreateLevel(); });
	Thread1.join();
}