#pragma once

#include "Model.h"
#include "Math.h"

class SceneNode
{
private:

	
	Model*						m_pModel;
	vector<SceneNode*>			m_Children;

	float						m_x, m_y, m_z;
	float						m_xangle, m_yangle, m_zangle;
	float						m_scale;

	float						m_worldCentreX, m_worldCentreY, m_worldCentreZ, m_worldScale;

	bool						m_canCollide, m_isVisable;

	XMMATRIX					m_localWorldMatrix;


public:

	SceneNode();
	~SceneNode();
	void Execute(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);

	void UpdateCollisionTree(XMMATRIX* world, float scale);
	bool CheckCollision(SceneNode* node); //Call to check for collisions
	bool CheckCollision(SceneNode* node, SceneNode* rootNode); //Called within its own function or to start a collision check down the tree
	bool CheckRaycastCollision(xyz rayPosition, xyz rayDirection, bool checkChildren);

	float GetXPos();
	float GetYPos();
	float GetZPos();
	float GetRotationX();
	float GetRotationY();
	float GetRotationZ();
	float GetScale();
	bool GetObjectCollideState();
	XMVECTOR GetWorldCentrePos();
	ObjFileModel* GetModelObject();


	bool SetXPos(float x, SceneNode* rootNode);
	void SetXPos(float x);
	bool SetYPos(float y, SceneNode* rootNode);
	void SetYPos(float y);
	bool SetZPos(float z, SceneNode* rootNode);
	void SetZPos(float z);
	bool SetRotationX(float pitch, SceneNode* rootNode);
	bool SetRotationY(float yaw, SceneNode* rootNode);
	bool SetRotationZ(float roll, SceneNode* rootNode);
	bool SetScale(float scale, SceneNode* rootNode);
	void SetIsObjectDrawn(bool objectDrawn);
	void SetCanObjectCollide(bool objectCollide);

	bool IncXPos(float x, SceneNode* rootNode);
	bool IncYPos(float y, SceneNode* rootNode);
	bool IncZPos(float z, SceneNode* rootNode);
	bool IncRotX(float x, SceneNode* rootNode);
	bool IncRotY(float y, SceneNode* rootNode);
	bool IncRotZ(float z, SceneNode* rootNode);

	void AddChildNode(SceneNode* node);
	bool DetachChildNode(SceneNode* node);

	void AddModel(Model* model);

	void LookAtXYZ(float, float, float, SceneNode* rootNode);
};

