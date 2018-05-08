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
	float	m_dx, m_dy, m_dz;

	float	m_cameraYawRotation, m_cameraPitchRotation;

	XMVECTOR	m_position, m_lookat, m_up, m_right, m_forward;

	XMVECTOR	m_DefaultForward, m_DefaultRight;

	XMMATRIX	m_RotationMatrix;
public:

	Camera(float, float, float, float);
	~Camera();

	void Rotate(float degrees);
	void Pitch(float degrees);
	void Forward(float distance);
	void Strafe(float distance);
	void CalcSides();
	XMMATRIX GetViewMatrix();

	float GetX();
	float GetY();
	float GetZ();
	xyz GetCameraPos();
	xyz GetLookAt();
	xyz GetCameraRight();

};