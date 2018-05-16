#include "Input.h"


Input::Input()
{


}

Input::~Input()
{
	m_pDirectInput->Release();

	m_pKeyboardDevice->Unacquire();
	m_pKeyboardDevice->Release();

	m_pMouseDevice->Unacquire();
	m_pMouseDevice->Release();

}

//Initializes the input class
HRESULT Input::InitialiseInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	//Direct Input initialisation
	ZeroMemory(m_KeyboardKeyState, sizeof(m_KeyboardKeyState));
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	if (FAILED(hr))
		return hr;

	//Keyboard initialisation
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, NULL);
	if (FAILED(hr))
		return hr;

	//Set the keyboard data format
	hr = m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		return hr;

	//Set flags for when the keyboard is active
	hr = m_pKeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return hr;

	//Access keyboard
	hr = m_pKeyboardDevice->Acquire();
	if (FAILED(hr))
		return hr;

	//Mouse initialisation
	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL);
	if (FAILED(hr))
		return hr;

	//Sets Mouse data format
	hr = m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		return hr;

	//Sets the flags for the mouse
	hr = m_pMouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		return hr;

	m_pMouseDevice->Acquire();
	if (FAILED(hr))
		return hr;

	//Setup of XInput controller 
	ZeroMemory(&m_ControllerState, sizeof(m_ControllerState));
	
	ZeroMemory(&m_VibrationState, sizeof(m_VibrationState));

	return S_OK;
}

void Input::ReadInputStates()
{
	HRESULT hr;

	//Read the current keyboard state
	hr = m_pKeyboardDevice->GetDeviceState(sizeof(m_KeyboardKeyState), (LPVOID)&m_KeyboardKeyState);

	//If the HRESULT returns a negative, check to see if the keyboard was lost/not initialized and reacquire
	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) ||
			(hr == DIERR_NOTACQUIRED))
		{
			m_pKeyboardDevice->Acquire();
		}
	}

	//If the HRESULT returns a negative, check to see if the mouse was lost/not initialized and reacquire
	hr = m_pMouseDevice->GetDeviceState(sizeof(m_MouseState), (LPVOID)&m_MouseState);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) ||
			(hr == DIERR_NOTACQUIRED))
		{
			m_pMouseDevice->Acquire();
		}
	}

	//Gets the current controller state 
	DWORD ControllerStateResult = XInputGetState(0, &m_ControllerState);

	if (ControllerStateResult != ERROR_SUCCESS)
	{
		//Add a custom debug here
	}

}

//Checks to see if a key has been pressed
bool Input::IsKeyPressed(unsigned char DI_KEYCODE)
{
	return m_KeyboardKeyState[DI_KEYCODE] & 0x80;

	
}

bool Input::HasMouseMoved()
{
	if (m_MouseState.lX != m_LastMouseState.lX
		|| m_MouseState.lY != m_LastMouseState.lY)
	{
		return true;
	}
	else
		return false;
}

float Input::GetMouseX()
{
	return m_MouseState.lX - m_LastMouseState.lX;
}

void Input::UpdateMouse()
{
	m_LastMouseState = m_MouseState;
}

bool Input::IsButtonPressed(unsigned short BUTTON_CODE)
{
	return m_ControllerState.Gamepad.wButtons & BUTTON_CODE;
}

float Input::GetControllerLeftAnalogueX()
{
	return m_ControllerState.Gamepad.sThumbLX;
}

float Input::GetControllerLeftAnalogueY()
{
	return m_ControllerState.Gamepad.sThumbLY;
}

float Input::GetControllerRightAnalogueX()
{
	return m_ControllerState.Gamepad.sThumbRX;
}

float Input::GetLeftStickDirection()
{
	float tempMagnitude = 0;

	float tempLX = m_ControllerState.Gamepad.sThumbLX;
	float tempLY = m_ControllerState.Gamepad.sThumbLY;

	tempMagnitude = (tempLX * tempLX + tempLY * tempLY);

	return tempMagnitude;
}

void Input::SetControllerVibration(float left, float right, float seconds)
{
	Mutex1.lock();

	m_VibrationState.wLeftMotorSpeed = left;
	m_VibrationState.wRightMotorSpeed = right;
	XInputSetState(0, &m_VibrationState);

	Sleep(seconds * 1000);

	m_VibrationState.wLeftMotorSpeed = 0;
	m_VibrationState.wRightMotorSpeed = 0;
	XInputSetState(0, &m_VibrationState);

	Mutex1.unlock();
}