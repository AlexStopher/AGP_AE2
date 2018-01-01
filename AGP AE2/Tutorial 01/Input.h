#pragma once

#include <dinput.h>



class Input
{
private:

	IDirectInput8*			m_pDirectInput;
	IDirectInputDevice8*	m_pMouseDevice;
	IDirectInputDevice8*	m_pKeyboardDevice;
	DIMOUSESTATE			m_MouseState;
	unsigned char			m_KeyboardKeyState[256];

public:

	Input();
	~Input();

	HRESULT InitialiseInput(HINSTANCE hInst, HWND hWnd);
	void ReadInputStates();
	bool IsKeyPressed(unsigned char DI_KEYCODE);
};

