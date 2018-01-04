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

}

//Checks to see if a key has been pressed
bool Input::IsKeyPressed(unsigned char DI_KEYCODE)
{
	return m_KeyboardKeyState[DI_KEYCODE] & 0x80;
}



