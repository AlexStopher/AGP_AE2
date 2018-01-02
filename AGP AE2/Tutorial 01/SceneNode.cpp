#include "SceneNode.h"



SceneNode::SceneNode()
{
	m_pModel = NULL;

	m_x = 0;
	m_y = 0;
	m_z = 0;

	m_xangle = 0.0f;
	m_yangle = 0.0f;
	m_zangle = 0.0f;

	m_scale = 1.0f;

	m_canCollide = true;
	m_isVisable = true;

	m_localWorldMatrix = XMMatrixIdentity();

}


SceneNode::~SceneNode()
{

}

void SceneNode::Execute(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
	XMMATRIX localWorld = XMMatrixIdentity();

	localWorld = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	localWorld *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	localWorld *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	localWorld *= XMMatrixScaling(m_scale, m_scale, m_scale);
	localWorld *= XMMatrixTranslation(m_x, m_y, m_z);

	localWorld *= *world;

	if (m_pModel && m_isVisable == true)
	{
		m_pModel->Draw(&localWorld, view, projection);
	}
	
	//go through children and draw them
	for (int i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Execute(&localWorld, view, projection);
	}
}

void SceneNode::AddChildNode(SceneNode* node)
{
	m_Children.push_back(node);
}

bool SceneNode::DetachChildNode(SceneNode* node)
{
	for (int i = 0; i < m_Children.size(); i++)
	{
		if (node == m_Children[i])
		{
			m_Children.erase(m_Children.begin() + i);
			return true;
		}
	
		if (m_Children[i]->DetachChildNode(node) == true)
			return true;
	}

	return false;
}

void SceneNode::UpdateCollisionTree(XMMATRIX* world, float scale)
{
	XMVECTOR centreTemp;
	
	m_localWorldMatrix = XMMatrixIdentity();

	m_localWorldMatrix = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	m_localWorldMatrix *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	m_localWorldMatrix *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));
	m_localWorldMatrix *= XMMatrixScaling(m_scale, m_scale, m_scale);
	m_localWorldMatrix *= XMMatrixTranslation(m_x, m_y, m_z);

	m_localWorldMatrix *= *world;

	m_worldScale = scale * m_scale;

	if (m_pModel)
		centreTemp = XMVectorSet(m_pModel->GetBoundingSphereX(), m_pModel->GetBoundingSphereY(), m_pModel->GetBoundingSphereZ(), 0.0f);
	else
		centreTemp = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	centreTemp = XMVector3Transform(centreTemp, m_localWorldMatrix);
	m_worldCentreX = XMVectorGetX(centreTemp);
	m_worldCentreY = XMVectorGetY(centreTemp);
	m_worldCentreZ = XMVectorGetZ(centreTemp);

	for (int i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->UpdateCollisionTree(&m_localWorldMatrix, m_worldScale);
	}
}

bool SceneNode::CheckCollision(SceneNode* node)
{
	return CheckCollision(node, this);
}

bool SceneNode::CheckCollision(SceneNode* node, SceneNode* rootNode)
{
	//checks to see if the object can collide, and if not returns a false collision
	

	//checks to see if the root or children are being tested
	if (rootNode == node)
		return false;

	if (m_pModel && node->m_pModel)
	{
		XMVECTOR Object1 = GetWorldCentrePos();
		XMVECTOR Object2 = node->GetWorldCentrePos();

		float x1 = XMVectorGetX(Object1);
		float x2 = XMVectorGetX(Object2);
		float y1 = XMVectorGetY(Object1);
		float y2 = XMVectorGetY(Object2);
		float z1 = XMVectorGetZ(Object1);
		float z2 = XMVectorGetZ(Object2);

		float distanceSquared = (pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));

	

		if (distanceSquared < pow((this->m_pModel->GetBoundingSphereRadius() * m_worldScale) +
			(node->m_pModel->GetBoundingSphereRadius() * node->m_worldScale), 2) &&
			this->m_canCollide == true && node->m_canCollide == true)
		{

			//checks the node against the current object for collision
			for (int i = 0; i < node->m_pModel->GetModelObject()->numverts; i += 3)
			{
				XMVECTOR p1, p2, p3;

				p1 = XMVectorSet(node->m_pModel->GetModelObject()->vertices[i].Pos.x,
					node->m_pModel->GetModelObject()->vertices[i].Pos.y,
					node->m_pModel->GetModelObject()->vertices[i].Pos.z, 0.0f);

				p2 = XMVectorSet(node->m_pModel->GetModelObject()->vertices[i + 1].Pos.x,
					node->m_pModel->GetModelObject()->vertices[i + 1].Pos.y,
					node->m_pModel->GetModelObject()->vertices[i + 1].Pos.z, 0.0f);

				p3 = XMVectorSet(node->m_pModel->GetModelObject()->vertices[i + 2].Pos.x,
					node->m_pModel->GetModelObject()->vertices[i + 2].Pos.y,
					node->m_pModel->GetModelObject()->vertices[i + 2].Pos.z, 0.0f);

				p1 = XMVector3Transform(p1, node->m_localWorldMatrix);
				p2 = XMVector3Transform(p2, node->m_localWorldMatrix);
				p3 = XMVector3Transform(p3, node->m_localWorldMatrix);

				xyz point1, point2, point3, Ray;

				point1.x = XMVectorGetX(p1);
				point1.y = XMVectorGetY(p1);
				point1.z = XMVectorGetZ(p1);

				point2.x = XMVectorGetX(p2);
				point2.y = XMVectorGetY(p2);
				point2.z = XMVectorGetZ(p2);

				point3.x = XMVectorGetX(p3);
				point3.y = XMVectorGetY(p3);
				point3.z = XMVectorGetZ(p3);



				Ray.x = point2.x - point1.x;
				Ray.y = point2.y - point1.y;
				Ray.z = point2.z - point1.z;

				Plane plane = Math::PlaneCreation(&point1, &point2, &point3);

				if (CheckRaycastCollision(point1, Ray, false) == true)
				{
					return true;
				}

				Ray.x = point3.x - point2.x;
				Ray.y = point3.y - point2.y;
				Ray.z = point3.z - point2.z;

				if (CheckRaycastCollision(point2, Ray, false) == true)
				{
					return true;
				}

				Ray.x = point3.x - point2.x;
				Ray.y = point3.y - point2.y;
				Ray.z = point3.z - point2.z;

				if (CheckRaycastCollision(point3, Ray, false) == true)
				{
					return true;
				}


			}

			//Checks the current triangle against the node
			for (int i = 0; i < m_pModel->GetModelObject()->numverts; i += 3)
			{
				XMVECTOR p1, p2, p3;

				p1 = XMVectorSet(m_pModel->GetModelObject()->vertices[i].Pos.x,
					m_pModel->GetModelObject()->vertices[i].Pos.y,
					m_pModel->GetModelObject()->vertices[i].Pos.z, 0.0f);

				p2 = XMVectorSet(m_pModel->GetModelObject()->vertices[i + 1].Pos.x,
					m_pModel->GetModelObject()->vertices[i + 1].Pos.y,
					m_pModel->GetModelObject()->vertices[i + 1].Pos.z, 0.0f);

				p3 = XMVectorSet(m_pModel->GetModelObject()->vertices[i + 2].Pos.x,
					m_pModel->GetModelObject()->vertices[i + 2].Pos.y,
					m_pModel->GetModelObject()->vertices[i + 2].Pos.z, 0.0f);

				p1 = XMVector3Transform(p1, m_localWorldMatrix);
				p2 = XMVector3Transform(p2, m_localWorldMatrix);
				p3 = XMVector3Transform(p3, m_localWorldMatrix);

				xyz point1, point2, point3, Ray;

				point1.x = XMVectorGetX(p1);
				point1.y = XMVectorGetY(p1);
				point1.z = XMVectorGetZ(p1);

				point2.x = XMVectorGetX(p2);
				point2.y = XMVectorGetY(p2);
				point2.z = XMVectorGetZ(p2);

				point3.x = XMVectorGetX(p3);
				point3.y = XMVectorGetY(p3);
				point3.z = XMVectorGetZ(p3);



				Ray.x = point2.x - point1.x;
				Ray.y = point2.y - point1.y;
				Ray.z = point2.z - point1.z;

				if (node->CheckRaycastCollision(point1, Ray, false) == true)
				{
					return true;
				}

				Ray.x = point3.x - point2.x;
				Ray.y = point3.y - point2.y;
				Ray.z = point3.z - point2.z;

				if (node->CheckRaycastCollision(point2, Ray, false) == true)
				{
					return true;
				}

				Ray.x = point1.x - point3.x;
				Ray.y = point1.y - point3.y;
				Ray.z = point1.z - point3.z;

				if (node->CheckRaycastCollision(point3, Ray, false) == true)
				{
					return true;
				}


			}
		}

		return false;
}
	
	//go through compared nodes
	for (int i = 0; i < node->m_Children.size(); i++)
	{
		if (CheckCollision(node->m_Children[i], rootNode) == true)
			return true;
	}

	//go through composite object children
	for (int i = 0; i < m_Children.size(); i++)
	{
		if (m_Children[i]->CheckCollision(node, rootNode) == true)
			return true;
	}

	return false;
}

bool SceneNode::CheckRaycastCollision(xyz rayPosition, xyz rayDirection, bool checkChildren)
{

	if (GetObjectCollideState() == false)
		return false;

	if (m_pModel)
	{
		bool inRange;


		//Gets the Distance to the node from the node position
	


		float Distance = sqrt(pow((m_x - rayPosition.x), 2) + pow((m_y - rayPosition.y), 2) + pow((m_z - rayPosition.z), 2));

		//Gets the Length of the ray
		float x = rayDirection.x;
		float y = rayDirection.y;
		float z = rayDirection.z;
		float radiusSum, rayLength;

		rayLength = sqrt((x*x) + (y*y) + (z*z));
		


		//Gets the sum of the Radius and the Ray length
		radiusSum = (m_pModel->GetBoundingSphereRadius() + rayLength);

		if (Distance > radiusSum)
		{
			//Node too far away to collide
		}
		else
		{
			for (int i = 0; i < m_pModel->GetModelObject()->numverts; i+=3)
			{
				XMVECTOR p1, p2, p3;

				p1 = XMVectorSet(m_pModel->GetModelObject()->vertices[i].Pos.x,
					m_pModel->GetModelObject()->vertices[i].Pos.y,
					m_pModel->GetModelObject()->vertices[i].Pos.z, 0.0f);

				p2 = XMVectorSet(m_pModel->GetModelObject()->vertices[i + 1].Pos.x,
					m_pModel->GetModelObject()->vertices[i + 1].Pos.y,
					m_pModel->GetModelObject()->vertices[i + 1].Pos.z, 0.0f);

				p3 = XMVectorSet(m_pModel->GetModelObject()->vertices[i + 2].Pos.x,
					m_pModel->GetModelObject()->vertices[i + 2].Pos.y,
					m_pModel->GetModelObject()->vertices[i + 2].Pos.z, 0.0f);

				p1 = XMVector3Transform(p1, m_localWorldMatrix);
				p2 = XMVector3Transform(p2, m_localWorldMatrix);
				p3 = XMVector3Transform(p3, m_localWorldMatrix);

				xyz point1, point2, point3;

				point1.x = XMVectorGetX(p1);
				point1.y = XMVectorGetY(p1);
				point1.z = XMVectorGetZ(p1);

				point2.x = XMVectorGetX(p2);
				point2.y = XMVectorGetY(p2);
				point2.z = XMVectorGetZ(p2);

				point3.x = XMVectorGetX(p3);
				point3.y = XMVectorGetY(p3);
				point3.z = XMVectorGetZ(p3);

				Plane plane = Math::PlaneCreation(&point1, &point2, &point3);

				float rayStartCol, rayEndCol;

				rayStartCol = Math::PlaneEquation(&plane, &rayPosition);
				rayEndCol = Math::PlaneEquation(&plane, &(rayPosition + rayDirection));

				if (Math::Sign(rayStartCol) != Math::Sign(rayEndCol))
				{
					xyz Position;

					Position = Math::PlaneIntersection(&plane, &rayPosition, &(rayPosition + rayDirection));

					if (Math::PointInTriangle(&point1, &point2, &point3, &Position) == true)
						return true;
					
				}

			}
		}
	}

	if (checkChildren == true)
	{
		//check children nodes 
		for (int i = 0; i < m_Children.size(); i++)
		{
			if (m_Children[i]->CheckRaycastCollision(rayPosition, rayDirection, false) == true)
				return true;
		}
	}

	return false;
}



//get methods

bool SceneNode::GetObjectCollideState()
{
	return m_canCollide;
}

float SceneNode::GetXPos()
{
	return m_x;
}

float SceneNode::GetYPos()
{
	return m_y;
}

float SceneNode::GetZPos()
{
	return m_z;
}

float SceneNode::GetRotationX()
{
	return m_xangle;
}

float SceneNode::GetRotationY()
{
	return m_yangle;
}

float SceneNode::GetRotationZ()
{
	return m_zangle;
}

float SceneNode::GetScale()
{
	return m_scale;
}

XMVECTOR SceneNode::GetWorldCentrePos()
{
	return XMVectorSet(m_worldCentreX, m_worldCentreY, m_worldCentreZ, 0.0f);
}

ObjFileModel* SceneNode::GetModelObject()
{
	return m_pModel->GetModelObject();
}


//set methods

bool SceneNode::SetXPos(float x, SceneNode* rootNode)
{

	float oldX = m_x;
	m_x = x;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	
	if (CheckCollision(rootNode) == true)
	{
		m_x = oldX;
		return true;
	}

	return false;

}

void SceneNode::SetXPos(float x)
{
	m_x = x;
}

bool SceneNode::SetYPos(float y, SceneNode* rootNode)
{
	float oldY = m_y;
	m_y = y;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_y = oldY;
		return true;
	}

	return false;
}

void SceneNode::SetYPos(float y)
{
	m_y = y;
}

bool SceneNode::SetZPos(float z, SceneNode* rootNode)
{

	float oldZ = m_z;
	m_z = z;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_z = oldZ;
		return true;
	}

	return false;
}

void SceneNode::SetZPos(float z)
{
	m_z = z;
}

bool SceneNode::SetRotationX(float pitch, SceneNode* rootNode)
{
	float oldxangle = m_xangle;
	m_xangle = pitch;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_xangle = oldxangle;
		return true;
	}

	return false;
}


bool SceneNode::SetRotationY(float yaw, SceneNode* rootNode)
{
	float oldyangle = m_yangle;
	m_yangle = yaw;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_xangle = oldyangle;
		return true;
	}

	return false;
}

bool SceneNode::SetRotationZ(float roll, SceneNode* rootNode)
{
	float oldzangle = m_zangle;
	m_zangle = roll;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	if (CheckCollision(rootNode) == true)
	{
		m_zangle = oldzangle;
		return true;
	}

	return false;
}

bool SceneNode::SetScale(float scale, SceneNode* rootNode)
{
	float oldscale = m_scale;
	m_scale = scale;

	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	if (CheckCollision(rootNode) == true)
	{
		m_scale = oldscale;
		return true;
	}

	m_pModel->CalculateBoundingSphereRadius();

	return false;
}

void SceneNode::SetIsObjectDrawn(bool objectDrawn)
{
	m_isVisable = objectDrawn;
}

void SceneNode::SetCanObjectCollide(bool objectCollide)
{
	m_canCollide = objectCollide;
}

void SceneNode::AddModel(Model* model)
{
	m_pModel = model;

}

//Increase methods

bool SceneNode::IncXPos(float x, SceneNode* rootNode)
{
	float oldX = m_x;
	m_x += x;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_x = oldX;
		return true;
	}

	return false;
}

bool SceneNode::IncYPos(float y, SceneNode* rootNode)
{

	float oldY = m_y;
	m_y += y;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_y = oldY;
		return true;
	}

	return false;
}

bool SceneNode::IncZPos(float z, SceneNode* rootNode)
{

	float oldZ = m_z;
	m_z += z;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_z = oldZ;
		return true;
	}

	return false;
}

bool SceneNode::IncRotX(float x, SceneNode* rootNode)
{
	float oldxangle = m_xangle;
	m_xangle += x;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_xangle = oldxangle;
		return true;
	}

	return false;
}

bool SceneNode::IncRotY(float y, SceneNode* rootNode)
{
	float oldyangle = m_yangle;
	m_yangle += y;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);


	if (CheckCollision(rootNode) == true)
	{
		m_xangle = oldyangle;
		return true;
	}

	return false;
}

bool SceneNode::IncRotZ(float z, SceneNode* rootNode)
{
	float oldzangle = m_zangle;
	m_zangle += z;

	//system requires all of the tree be updated(change in future)
	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	if (CheckCollision(rootNode) == true)
	{
		m_zangle = oldzangle;
		return true;
	}

	return false;
}

void SceneNode::LookAtXYZ(float x, float y, float z, SceneNode* rootNode)
{
	float dx = x - m_x;
	float dz = z - m_z;
	float dy = y - m_y;

	//Get the hypotenuse of xz
	float dxz = sqrt((dx * dx) + (dz*dz));

	m_xangle = -atan2(dy, dxz) * (180 / XM_PI);
	m_yangle = atan2(dx, dz) * (180 / XM_PI);

}

bool SceneNode::MoveForward(float distance, SceneNode* rootNode)
{
	float oldx = m_x;
	float oldy = m_y;
	float oldz = m_z;
	
	m_x += sin(m_yangle * (XM_PI / 180)) * distance * cos(m_xangle * (XM_PI / 180));
	m_y += -sin(m_xangle * (XM_PI / 180)) * distance;
	m_z += cos(m_yangle * (XM_PI / 180)) * distance * cos(m_xangle * (XM_PI / 180));

	XMMATRIX identity = XMMatrixIdentity();
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	if (CheckCollision(rootNode) == true)
	{
		m_x = oldx;
		m_y = oldy;
		m_z = oldz;

		return true;
	}

	return false;
}