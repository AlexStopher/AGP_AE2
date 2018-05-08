#include "Camera.h"

Camera::Camera(float x, float y, float z, float cameraYRotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_cameraYawRotation = cameraYRotation;
	m_RotationMatrix = XMMatrixIdentity();
	
	m_DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	m_DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	CalcSides();

	GetViewMatrix();
	

}

Camera::~Camera()
{

}

void Camera::Rotate(float degrees)
{

	m_cameraYawRotation += degrees;

	CalcSides();
}

void Camera::Pitch(float degrees)
{

	m_cameraPitchRotation += degrees;


}

void Camera::Forward(float distance)
{
	m_x += m_dx * distance;
	m_z += m_dz * distance;
}

//Increase / Decrease the X position of the camera relative to the forward of the model
void Camera::Strafe(float distance)
{
	XMVECTOR tempPos = XMVectorSet(m_x, m_y, m_z, 0);
	tempPos += m_right * -distance;

	m_x = XMVectorGetX(tempPos);
	m_z = XMVectorGetZ(tempPos);
}

//Works out the dx and dz of the triangle
void Camera::CalcSides()
{
	m_dz = cos(m_cameraYawRotation *(XM_PI / 180));
	m_dx = sin(m_cameraYawRotation *(XM_PI / 180 ));
}


XMMATRIX Camera::GetViewMatrix()
{
	//
	//m_RotationMatrix = XMMatrixRotationRollPitchYaw(m_cameraPitchRotation, m_cameraYawRotation, 0.0f);
	//m_lookat = XMVector3TransformCoord(m_DefaultForward, m_RotationMatrix);
	//m_lookat = XMVector3Normalize(m_lookat);

	//XMMATRIX tempYRot;
	//tempYRot = XMMatrixRotationY(m_cameraYawRotation);

	//m_right = XMVector3TransformCoord(m_DefaultRight, tempYRot);
	//m_up = XMVector3TransformCoord(m_up, tempYRot);
	//m_forward = XMVector3TransformCoord(m_DefaultForward, tempYRot);

	//

	//m_lookat = m_position + m_lookat;
	//

	m_position = XMVectorSet(m_x, m_y, m_z, 0.0f);
	m_lookat = XMVectorSet(m_x + m_dx, m_y, m_z + m_dz, 0.0f);

	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_forward = XMVector3Normalize(m_lookat - m_position);
	m_right = XMVector3Cross(m_up, m_forward);
	
	XMMATRIX view = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return view;
}

float Camera::GetX()
{
	return m_x;
}

float Camera::GetY()
{
	return m_y;
}

float Camera::GetZ()
{
	return m_z;
}

xyz Camera::GetCameraPos()
{
	xyz temp(m_x, m_y, m_z);
	return temp;
}

xyz Camera::GetLookAt()
{
	xyz temp(m_dx, 0, m_dz);
	//xyz temp(XMVectorGetX(m_lookat), XMVectorGetY(m_lookat), XMVectorGetZ(m_lookat));

	return temp;
}

xyz Camera::GetCameraRight()
{
	xyz temp(XMVectorGetX(m_right), XMVectorGetY(m_right), XMVectorGetZ(m_right));
	return temp;
}