#include "Camera.h"

Camera::Camera(float x, float y, float z, float cameraYRotation)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_cameraRotation = cameraYRotation;

	CalcSides();

	//turn into function calls
	GetViewMatrix();
	

}

Camera::~Camera()
{

}

void Camera::Rotate(float degrees)
{

	m_cameraRotation += degrees;

	CalcSides();
}

void Camera::Forward(float distance)
{
	m_x += m_dx * distance;
	m_z += m_dz * distance;
}

//Works out the dx and dz of the triangle
void Camera::CalcSides()
{
	m_dz = cos(m_cameraRotation *(XM_PI / 180));
	m_dx = sin(m_cameraRotation *(XM_PI / 180 ));
}

XMMATRIX Camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0f);
	m_lookat = XMVectorSet(m_x + m_dx, m_y, m_z + m_dz, 0.0f);
	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

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