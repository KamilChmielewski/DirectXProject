#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <algorithm>
//Texture loading
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

class Texture
{
private:
	ID3D11ShaderResourceView*	_pTextureRV;
	ID3D11Device*				_pd3dDevice;

public:
	Texture(ID3D11Device* _pd3dDevice);
	~Texture();
	void InitializeTexture(const wchar_t* filePath);
	ID3D11ShaderResourceView* GetTexture() { return _pTextureRV; }
};