#include "GameManager.h"

struct POS_COL_TEX_NORM_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};

struct CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;
	XMVECTOR DirectionalLightVector;
	XMVECTOR DirectionalLightColour;
	XMVECTOR DirectionalLightVector2;
	XMVECTOR DirectionalLightColour2;
	XMVECTOR AmbientLightColour;
	XMVECTOR PointLightPosition;
	XMVECTOR PointLightColour;
};

GameManager::GameManager()
{
	m_pPlayerInput = new Input;
}


GameManager::~GameManager()
{
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
	char Name[100] = "Alex's Project";

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
	RECT rc = { 0,0,640,480 };
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
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

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
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;

	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&g_pBackBufferRTView);

	pBackBufferTexture->Release();



	if (FAILED(hr)) return hr;

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
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	//Creates the actual Z Buffer
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();


	// Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	m_2DText = new Text2D("assets/font1.png", g_pD3DDevice, g_pImmediateContext);

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

	g_pD3DDevice->CreateBlendState(&b, &m_pBlendAlphaEnable);

	return S_OK;
}


void GameManager::ShutdownD3D()
{
	//if (g_pModel) g_pModel->~Model();
	if (g_pSampler0) g_pSampler0->Release();
	if (g_pTexture0) g_pTexture0->Release();
	if (g_camera) g_camera->~Camera();
	if (g_pZBuffer) g_pZBuffer->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pConstantBuffer0) g_pConstantBuffer0->Release();

	if (m_pPlayerInput) m_pPlayerInput->~Input();

	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();

}

HRESULT GameManager::InitialiseGraphics()
{
	HRESULT hr = S_OK;

	g_pModel = new Model(g_pD3DDevice, g_pImmediateContext);
	g_pModel->LoadObjModel("assets/Wall.obj");
	g_pModel->LoadShader("model_shaders.hlsl");
	g_pModel->AddTexture("assets/texture.bmp");

	g_pModel2 = new Model(g_pD3DDevice, g_pImmediateContext);
	g_pModel2->LoadObjModel("assets/cube.obj");
	g_pModel2->LoadShader("model_shaders.hlsl");
	g_pModel2->AddTexture("assets/texture.bmp");

	g_pModel3 = new Model(g_pD3DDevice, g_pImmediateContext);
	g_pModel3->LoadObjModel("assets/cube.obj");
	g_pModel3->LoadShader("model_shaders.hlsl");
	g_pModel3->AddTexture("assets/texture.bmp");

	RootNode = new SceneNode();
	node1 = new SceneNode();
	node2 = new SceneNode();
	cameraNode = new SceneNode();

	RootNode->AddChildNode(node1);
	RootNode->AddChildNode(node2);
	//RootNode->AddChildNode(cameraNode);

	node1->AddModel(g_pModel);
	node2->AddModel(g_pModel2);

	node1->SetZPos(20.0f, RootNode);
	node2->SetZPos(10.0f, RootNode);
	node2->SetXPos(3.0f, RootNode);

	node1->SetRotationY(180, RootNode);
	node1->SetScale(2, RootNode);
	//cameraNode->AddModel(g_pModel3);

	//cameraNode->SetCanObjectCollide(false);

	//Define vertices of a triangle
	POS_COL_TEX_NORM_VERTEX vertices[] =
	{
		//Cube

		//front face
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f),	 XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f),	 XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },
		{ XMFLOAT3(-1.0f,-1.0f,-1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f),	 XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },

		{ XMFLOAT3(1.0f,1.0f,-1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },
		{ XMFLOAT3(1.0f,-1.0f,-1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },
		{ XMFLOAT3(-1.0f,-1.0f,-1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f,0.0f,-1.0f) },

	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // GPU and CPU
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer);

	if (FAILED(hr))
	{
		return hr;
	}

	//Create constant buffer

	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 176;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &g_pConstantBuffer0);

	if (FAILED(hr))
	{
		return hr;
	}

	//Copy the certices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//Lock the buffer to allow writing
	g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));

	//Unlock of Buffer
	g_pImmediateContext->Unmap(g_pVertexBuffer, NULL);

	//Load and compile pixel and vertex shaders
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	hr = g_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);

	if (FAILED(hr))
	{
		return 0;
	}

	hr = g_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);

	if (FAILED(hr))
	{
		return 0;
	}

	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);

	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);


	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = g_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &g_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	g_camera = new Camera(0.0f, 0.0f, 0.0, 0.0f);

	return S_OK;

}

//Render the frame "Main" update loop for the buffer
void GameManager::RenderFrame(void)
{
	float rgba_clear_colour[4] = { 0.1f, 0.2f,0.6f, 1.0f };

	m_pPlayerInput->ReadInputStates();

	XMMATRIX identity = XMMatrixIdentity();

	RootNode->UpdateCollisionTree(&identity, 1.0f);


	if (m_pPlayerInput->IsKeyPressed(DIK_W))
	{
		g_camera->Forward(0.001f);

		xyz Lookat = g_camera->GetLookAt();

		Lookat.x *= 0.001f;
		Lookat.y *= 0.001f;
		Lookat.z *= 0.001f;

		if (RootNode->CheckRaycastCollision(g_camera->GetCameraPos(), Lookat, true) == true)
		{

			g_camera->Forward(-0.001f);
		}

	}

	if (m_pPlayerInput->IsKeyPressed(DIK_A))
		g_camera->Rotate(-0.04f);

	if (m_pPlayerInput->IsKeyPressed(DIK_D))
		g_camera->Rotate(0.04f);

	if (m_pPlayerInput->IsKeyPressed(DIK_S))
	{
		g_camera->Forward(-0.001f);

		xyz Lookat = g_camera->GetLookAt();

		Lookat.x *= -0.001f;
		Lookat.y *= -0.001f;
		Lookat.z *= -0.001f;

		if (RootNode->CheckRaycastCollision(g_camera->GetCameraPos(), Lookat, true) == true)
		{
			g_camera->Forward(0.001f);
		}

	}

	if (m_pPlayerInput->IsKeyPressed(DIK_K))
		node1->IncXPos(0.001f, RootNode);

	if (m_pPlayerInput->IsKeyPressed(DIK_H))
		node1->IncXPos(-0.001f, RootNode);

	if (m_pPlayerInput->IsKeyPressed(DIK_J))
		node1->IncZPos(-0.001f, RootNode);

	if (m_pPlayerInput->IsKeyPressed(DIK_U))
		node1->IncZPos(0.001f, RootNode);

	if (m_pPlayerInput->IsKeyPressed(DIK_I))
		node1->IncRotY(0.01f, RootNode);

	//Clear the back buffer

	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);

	//clear the Z Buffer
	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Render here
	m_2DText->AddText("ScaryZone", -1.0f, +1.0f, 0.1f);


	//Select primitive type
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//Constant buffer values
	CONSTANT_BUFFER0 cb0_values;
	XMMATRIX transpose;

	XMMATRIX world, projection, view, inverse;
	XMVECTOR determinant;

	world = XMMatrixIdentity();

	//Matrix that represents the field of view
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 640.0f / 480.0f, 0.0001f, 100.0f);

	//Camera view point
	view = g_camera->GetViewMatrix();


	//cb0_values.PointLightPosition = XMVector2Transform(g_point_light_position, inverse);

	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer0, 0, 0, &cb0_values, 0, 0);



	g_pModel->SetDirectionalLight(0.0f, 0.0f, -1.0f, 0.0f);


	g_pModel2->SetDirectionalLight(0.0f, 0.0f, -1.0f, 0.0f);


	RootNode->Execute(&world, &view, &projection);

	g_pImmediateContext->OMSetBlendState(m_pBlendAlphaEnable, 0, 0xffffffff);
	m_2DText->RenderText();
	g_pImmediateContext->OMSetBlendState(m_pBlendAlphaDisable, 0, 0xffffffff);

	g_pSwapChain->Present(0, 0);
}