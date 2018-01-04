#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>

struct Particle 
{
	float gravity;
	XMFLOAT3 position;
	XMFLOAT3 velocity;
	XMFLOAT3 colour;
};

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection; // 64 bytes
	XMMATRIX WorldView;
	XMVECTOR AmbientLightColour = { 0.1f, 0.1f, 0.1f, 0.1f }; // 16
	XMVECTOR DirectionalLightColour = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMVECTOR DirectionalLightVector;
	XMVECTOR PointLightPosition;
	XMVECTOR PointLightColour;
};

class ParticleGenerator
{
private:

	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;

	ID3D11VertexShader*			m_pVertexShader;
	ID3D11PixelShader*			m_pPixelShader;

	ID3D11Buffer*			    m_pVertexBuffer;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11InputLayout*			m_pInputLayout;

	ID3D11ShaderResourceView*	m_pTexture0;
	ID3D11SamplerState*			m_pSampler0;

	XMVECTOR					m_DirectionalLightVector;
	XMVECTOR					m_PointLightPosition;
	XMVECTOR					m_PointLightColour;


	char*						m_ShaderFilename;

	float						m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z;
	float						m_bounding_sphere_radius;

	float						m_scale;

	//private functions


public:

	ParticleGenerator(ID3D11Device*, ID3D11DeviceContext*);
	~ParticleGenerator();

	void Draw(XMMATRIX*, XMMATRIX*, XMMATRIX*); //Draw the Model using the World View Perpsective Matrix
	int AddTexture(char* filename); //Add a Texture to be loaded onto the Model
	
	int LoadShader(char* filename);  //Loads a shader onto the model class
	void SetDirectionalLight(float x, float y, float z, float w);
	void SetPointLight(float x, float y, float z, float w);
	void SetPointLightColour(float x, float y, float z, float w);


};
