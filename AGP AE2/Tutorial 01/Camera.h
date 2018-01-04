#pragma once

#include <d3d11.h>
#include <math.h>
#include "Math.h"


#define _XM_NO_INTRINSICS
#define XM_NO_ALIGNMENT

#include <xnamath.h>


class Camera
{
private:

	float	m_x, m_y, m_z;
	float	m_dx, m_dz;

	float	m_cameraRotation;

	XMVECTOR	m_position, m_lookat, m_up, m_right;


public:

	Camera(float, float, float, float);
	~Camera();

	void Rotate(float degrees);
	void Forward(float distance);
	void Left(float distance);
	void Right(float distance);
	void CalcSides();
	XMMATRIX GetViewMatrix();

	float GetX();
	float GetY();
	float GetZ();
	xyz GetCameraPos();
	xyz GetLookAt();
};