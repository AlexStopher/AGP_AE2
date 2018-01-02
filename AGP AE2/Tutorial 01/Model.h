#pragma once

#include "objfilemodel.h"


class Model
{
private:

	ID3D11Device*				m_pD3DDevice;
	ID3D11DeviceContext*		m_pImmediateContext;

	ObjFileModel*				m_pObject;
	ID3D11VertexShader*			m_pVertexShader;
	ID3D11PixelShader*			m_pPixelShader;
	ID3D11Buffer*				m_pConstantBuffer;
	ID3D11InputLayout*			m_pInputLayout;

	ID3D11ShaderResourceView*	m_pTexture0;
	ID3D11SamplerState*			m_pSampler0;

	XMVECTOR					m_DirectionalLightVector;
	

	char*						m_ShaderFilename;

	float						m_x, m_y, m_z;
	float						m_xangle, m_yangle, m_zangle;
	float						m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z;
	float						m_bounding_sphere_radius;

	float						m_scale;

	//private functions
	

	XMVECTOR GetBoundingSphereWorldSpacePosition();
	

public:

	

	Model(ID3D11Device*, ID3D11DeviceContext*);
	~Model();

	int LoadObjModel(char* filename); //Load the Obj file data
	void Draw(XMMATRIX*, XMMATRIX*, XMMATRIX*); //Draw the Model using the World View Perpsective Matrix
	int AddTexture(char* filename); //Add a Texture to be loaded onto the Model
	int AddLighting();

	int LoadShader(char* filename);  //Loads a shader onto the model class
	void SetDirectionalLight(float x, float y, float z, float w);

	float GetBoundingSphereX();
	float GetBoundingSphereY();
	float GetBoundingSphereZ();
	ObjFileModel* GetModelObject();

	void CalculateModelCentrePoint();
	void CalculateBoundingSphereRadius();

	void LookAtXYZ(float x, float y, float z);
	void MoveForward(float distance); //Move in the Models forward vector by a float number
	bool CheckCollision(Model* model);

	float GetBoundingSphereRadius();
	
};

