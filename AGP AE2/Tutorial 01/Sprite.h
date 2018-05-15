#pragma once

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>

const int SpriteShape = 6;

struct TexturePos
{
	float x;
	float y;
	float scale;
};

struct POS_TEX_VERTEXX
{
	XMFLOAT3 Pos;
	XMFLOAT2 Texture;
};


class Sprite
{
private:

	ID3D11Device * m_pD3DDevice;
	ID3D11DeviceContext* m_pImmediateContext;


	TexturePos		imagePos;

	ID3D11VertexShader*			m_pVertexShader;
	ID3D11PixelShader*			m_pPixelShader;
	ID3D11InputLayout*			m_pInputLayout;
	ID3D11ShaderResourceView*	m_pTexture0;
	ID3D11SamplerState*			m_pSampler0;
	ID3D11Buffer*			    m_pVertexBuffer;

	char*						m_pTextureFilename;

	ID3D11DepthStencilState* pDepthEnabledStencilState;		// state to turn on Z buffer
	ID3D11DepthStencilState* pDepthDisabledStencilState;	// state to turn off Z buffer

	int m_ScreenHeight;
	int m_ScreenWidth;

public:

	Sprite(char* filename, ID3D11Device* D3DDevice, ID3D11DeviceContext* ImmediateContext);
	~Sprite();

	void SetPosition(float x, float y, float scale);
	void Draw();
};

