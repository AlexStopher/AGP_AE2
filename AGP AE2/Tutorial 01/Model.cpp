#include "Model.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; // 64 bytes
	XMVECTOR AmbientLightColour = { 0.1f, 0.1f, 0.1f, 0.1f }; // 16
	XMVECTOR DirectionalLightColour = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMVECTOR DirectionalLightVector;
};

Model::Model(ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext)
{
	m_pD3DDevice		= D3DDevice;
	m_pImmediateContext = ImmediateContext;

	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;

	m_xangle = 0.0f;
	m_yangle = 0.0f;
	m_zangle = 0.0f;

	m_scale = 1.0f;

	m_ShaderFilename = NULL;
}


Model::~Model()
{
	delete m_pObject;

	m_pTexture0->Release();
	m_pSampler0->Release();
	m_pConstantBuffer->Release();
	m_pInputLayout->Release();
	m_pPixelShader->Release();
	m_pVertexShader->Release();
}

int Model::LoadObjModel(char* filename)
{
	HRESULT hr = S_OK;

	//Loads the object

	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);

	if (m_pObject->filename == "FILE NOT LOADED")
	{
		return S_FALSE;
	}

	//Creating the Buffer descriptor and setting it to the d3ddevice
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(MODEL_CONSTANT_BUFFER);
	constantBufferDesc.Usage	 = D3D11_USAGE_DEFAULT;

	hr = m_pD3DDevice->CreateBuffer(&constantBufferDesc, NULL, &m_pConstantBuffer);

	if (FAILED(hr))
	{
		return hr;
	}

	LoadShader(m_ShaderFilename);
	CalculateModelCentrePoint();
	CalculateBoundingSphereRadius();

	return S_OK;

}

void Model::Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
	
	XMMATRIX transpose = XMMatrixIdentity();

	MODEL_CONSTANT_BUFFER cb0_values;
	
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	cb0_values.WorldViewProjection = (*world) * (*view) * (*projection);

	//Lighting code
	cb0_values.DirectionalLightVector = m_DirectionalLightVector;


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

	m_pObject->Draw();
	m_pImmediateContext->Draw(sizeof(m_pObject->vertices), 0);
}

int Model::AddTexture(char* filename)
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

int Model::LoadShader(char* filename)
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

void Model::CalculateModelCentrePoint()
{
	float minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
	//reuse code for own project, only do min and max to get a vector that allows to get the radius and the sphere as the radius function is unoptimal
	for (int i = 0; i < m_pObject->numverts; i++)
	{
		//check for min and max of x
		if (m_pObject->vertices[i].Pos.x > maxx)
		{
			maxx = m_pObject->vertices[i].Pos.x;
		}
		else if (m_pObject->vertices[i].Pos.x < minx)
		{
			minx = m_pObject->vertices[i].Pos.x;
		}
		
		//check for min and max of y
		if (m_pObject->vertices[i].Pos.y > maxy)
		{
			maxy = m_pObject->vertices[i].Pos.y;
		}
		else if (m_pObject->vertices[i].Pos.y < miny)
		{
			miny = m_pObject->vertices[i].Pos.y;
		}

		//check for min and max of z
		if (m_pObject->vertices[i].Pos.z > maxz)
		{
			maxz = m_pObject->vertices[i].Pos.z;
		}
		else if (m_pObject->vertices[i].Pos.z < minz)
		{
			minz = m_pObject->vertices[i].Pos.z;
		}	
	}

	m_bounding_sphere_centre_x = (minx + maxx) / 2;
	m_bounding_sphere_centre_y = (miny + maxy) / 2;
	m_bounding_sphere_centre_z = (minz + maxz) / 2;

}

void Model::CalculateBoundingSphereRadius()
{

	
	float dz;
	float dx;

	float distance = 0;

	for (int i = 0; i < m_pObject->numverts; i++)
	{

		dz = m_pObject->vertices[i].Pos.z - m_bounding_sphere_centre_z;
		dx = m_pObject->vertices[i].Pos.x - m_bounding_sphere_centre_x;

		float vertDist = sqrt((dz * dz) + (dx * dx));
		
		if (vertDist > distance)
		{
			distance = vertDist;
		}
	}

	//BUG HERE
	m_bounding_sphere_radius = distance;

}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
{

	XMMATRIX world;
	XMMATRIX transpose = XMMatrixIdentity();


	world = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	world *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	world *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	world *= XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixTranslation(m_x, m_y, m_z);


	XMVECTOR offset;

	offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0);

	offset = XMVector3Transform(offset, world);

	return offset;
}

ObjFileModel* Model::GetModelObject()
{
	return m_pObject;
}

bool Model::CheckCollision(Model* model)
{
	if (model == this)
		return false;
	
	XMVECTOR Object1 = this->GetBoundingSphereWorldSpacePosition();
	XMVECTOR Object2 = model->GetBoundingSphereWorldSpacePosition();

	float x1 = XMVectorGetX(Object1);
	float x2 = XMVectorGetX(Object2);
	float y1 = XMVectorGetY(Object1);
	float y2 = XMVectorGetY(Object2);
	float z1 = XMVectorGetZ(Object1);
	float z2 = XMVectorGetZ(Object2);

	float distanceSquared = (pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));

	if(distanceSquared < pow(this->GetBoundingSphereRadius() + model->GetBoundingSphereRadius(), 2))
		return true; //collision


	return false;
}

int Model::AddLighting()
{
	return 0;
}

void Model::SetDirectionalLight(float x, float y, float z, float w)
{
	m_DirectionalLightVector = XMVectorSet(x, y, z, w);
}

void Model::LookAtXYZ(float x, float y, float z)
{
	float dx = x - m_x;
	float dz = z - m_z;
	float dy = y - m_y;

	//Get the hypotense of xz
	float dxz = sqrt((dx * dx) + (dz*dz));

	m_xangle = -atan2(dy, dxz) * (180 / XM_PI);
	m_yangle = atan2(dx, dz) * (180 / XM_PI);

}


#pragma region Get/Set Functions

float Model::GetBoundingSphereRadius()
{
	return m_bounding_sphere_radius;
}

float Model::GetBoundingSphereX()
{
	return m_bounding_sphere_centre_x;
}

float Model::GetBoundingSphereY()
{
	return m_bounding_sphere_centre_y;
}

float Model::GetBoundingSphereZ()
{
	return m_bounding_sphere_centre_z;
}
#pragma endregion