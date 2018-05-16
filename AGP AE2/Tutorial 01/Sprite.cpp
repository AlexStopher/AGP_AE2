#include "Sprite.h"


Sprite::Sprite(char* filename, ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext)
{
	m_pD3DDevice = D3DDevice;
	m_pImmediateContext = ImmediateContext;
	m_pTextureFilename = filename;

	m_ScreenHeight = 800;
	m_ScreenWidth = 1280;



		
	//Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										// Used by CPU and GPU
	bufferDesc.ByteWidth = sizeof(POS_TEX_VERTEXX) * 6;			// Total size of buffer,
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							// Use as a vertex buffer
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							// Allow CPU access
	HRESULT hr = D3DDevice->CreateBuffer(&bufferDesc, NULL, &m_pVertexBuffer);	// Create the buffer

	if (FAILED(hr)) exit(0);

	// Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("text2d_shaders.hlsl", 0, 0, "TextVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))exit(0);
	}

	hr = D3DX11CompileFromFile("text2d_shaders.hlsl", 0, 0, "TextPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) exit(0);
	}

	// Create shader objects
	hr = D3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVertexShader);
	if (FAILED(hr)) exit(0);

	hr = D3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPixelShader);
	if (FAILED(hr)) exit(0);

	// Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	hr = D3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);
	if (FAILED(hr)) exit(0);

	// Load in the texture from given filename
	hr = D3DX11CreateShaderResourceViewFromFile(D3DDevice, filename, NULL, NULL, &m_pTexture0, NULL);
	if (FAILED(hr)) exit(0);

	// Create sampler for texture
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = D3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler0);

	// Create 2 depth stencil states to turn Z buffer on and off
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = D3DDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthDisabledStencilState);
	if (FAILED(hr)) exit(0);

	depthStencilDesc.DepthEnable = true;
	hr = D3DDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthEnabledStencilState);
	if (FAILED(hr)) exit(0);


}


Sprite::~Sprite()
{

}

void Sprite::SetPosition(float x, float y, float scale)
{
	imagePos.x = x;
	imagePos.y = y;
	imagePos.scale = scale;
}

void Sprite::Draw()
{
	//change this to draw the UI in one image, Make modular later
	POS_TEX_VERTEXX vertices[] =
	{
		{XMFLOAT3(-0.5f, 0.5f, 5.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(0.5f, -0.5f, 5.0f), XMFLOAT2(1.0f, 1.0f)},
		{XMFLOAT3(-0.5f, -0.5f, 5.0f), XMFLOAT2(0.0f, 1.0f)},

		{XMFLOAT3(-0.5f, 0.5f, 5.0f), XMFLOAT2(0.0f, 0.0f)},
		{ XMFLOAT3(0.5f, 0.5f, 5.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 5.0f), XMFLOAT2(1.0f, 1.0f) }

	};


	
	D3D11_MAPPED_SUBRESOURCE ms;
	m_pImmediateContext->Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);	// Lock the buffer to allow writing
	memcpy(ms.pData, vertices, sizeof(vertices));						// Copy the data -  only upload those that are used
	m_pImmediateContext->Unmap(m_pVertexBuffer, NULL);

	// set all rendering states
	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);
	m_pImmediateContext->VSSetShader(m_pVertexShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	UINT stride = sizeof(POS_TEX_VERTEXX);
	UINT offset = 0;
	m_pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// turn off Z buffer so image is always on top
	m_pImmediateContext->OMSetDepthStencilState(pDepthDisabledStencilState, 1);


	m_pImmediateContext->Draw(6, 0);

	// turn on Z buffer so other rendering can use it
	m_pImmediateContext->OMSetDepthStencilState(pDepthEnabledStencilState, 1);
}