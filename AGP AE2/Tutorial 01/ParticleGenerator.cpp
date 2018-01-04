#include "ParticleGenerator.h"



ParticleGenerator::ParticleGenerator(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext)
{
	m_pD3DDevice = D3DDevice;
	m_pImmediateContext = ImmediateContext;


	m_scale = 1.0f;

	m_ShaderFilename = NULL;

}


ParticleGenerator::~ParticleGenerator()
{
}

void ParticleGenerator::Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{

	XMMATRIX transpose = XMMatrixIdentity();
	XMMATRIX inverse = XMMatrixIdentity();

	XMVECTOR determinant;

	MODEL_CONSTANT_BUFFER cb0_values;

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	cb0_values.WorldView = (*world) * (*view);
	cb0_values.WorldViewProjection = (*world) * (*view) * (*projection);

	//Lighting code

	cb0_values.DirectionalLightVector = m_DirectionalLightVector;


	//point light
	inverse = XMMatrixInverse(&determinant, *world);

	cb0_values.PointLightPosition = XMVector2Transform(m_PointLightPosition, inverse);
	cb0_values.PointLightColour = m_PointLightColour;

	transpose = XMMatrixTranspose(*world);

	cb0_values.DirectionalLightVector = XMVector3Transform(m_DirectionalLightVector, transpose);
	cb0_values.DirectionalLightVector = XMVector3Normalize(cb0_values.DirectionalLightVector);

	//Sets the model constant buffer as the active one



	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &cb0_values, 0, 0);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler0);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture0);



	m_pImmediateContext->VSSetShader(m_pVertexShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	
}

int ParticleGenerator::AddTexture(char* filename)
{
	HRESULT hr;

	hr = D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, filename, NULL, NULL, &m_pTexture0, NULL);

	if (FAILED(hr))
	{
		return hr;
	}

	//creates the sample descriptor and assigns the state to 
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));

	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler0);

	if (FAILED(hr))
	{
		return hr;
	}

	return 0;
}

int ParticleGenerator::LoadShader(char* filename)
{
	m_ShaderFilename = filename;

	HRESULT hr;

	ID3DBlob* VS, *PS, *Error;

	//Vertex Shader
	if (m_ShaderFilename == NULL)
	{
		hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_5_0", 0, 0, 0, &VS, &Error, 0);
	}
	else
	{
		hr = D3DX11CompileFromFile(m_ShaderFilename, 0, 0, "ModelVS", "vs_5_0", 0, 0, 0, &VS, &Error, 0);
	}

	if (Error != 0)
	{
		OutputDebugStringA((char*)Error->GetBufferPointer());
		Error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	//Pixel Shader
	if (m_ShaderFilename == NULL)
	{
		hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_5_0", 0, 0, 0, &PS, &Error, 0);
	}
	else
	{
		hr = D3DX11CompileFromFile(m_ShaderFilename, 0, 0, "ModelPS", "ps_5_0", 0, 0, 0, &PS, &Error, 0);
	}

	if (Error != 0)
	{
		OutputDebugStringA((char*)Error->GetBufferPointer());
		Error->Release();
		if (FAILED(hr))
		{
			return hr;
		}
	}


	//Setting the shaders to the members via the D3DDevice;
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVertexShader);

	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPixelShader);

	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->VSSetShader(m_pVertexShader, 0, 0);

	m_pImmediateContext->PSSetShader(m_pPixelShader, 0, 0);

	

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


	//Setting the input layout for the context
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA,0 }, // keep as 3 data bits?
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	return 0;
}

void ParticleGenerator::SetDirectionalLight(float x, float y, float z, float w)
{
	m_DirectionalLightVector = XMVectorSet(x, y, z, w);
}

void ParticleGenerator::SetPointLight(float x, float y, float z, float w)
{
	m_PointLightPosition = XMVectorSet(x, y, z, w);
}

void ParticleGenerator::SetPointLightColour(float x, float y, float z, float w)
{
	m_PointLightColour = XMVectorSet(x, y, z, w);
}
