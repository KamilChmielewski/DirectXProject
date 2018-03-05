#include "Texture.h"



Texture::Texture(ID3D11Device * _pd3dDevice)
{
	this->_pd3dDevice = _pd3dDevice;
	_pTextureRV = nullptr;
}

Texture::~Texture()
{
}

void Texture::InitializeTexture(const wchar_t* filePath)
{
	HRESULT hr;
	std::wstring fileType(filePath);
	std::wstring exten = fileType.substr(fileType.length() - 3, 3);

	if (exten == std::wstring(L"dds"))
	{
		hr = CreateDDSTextureFromFile(_pd3dDevice, filePath, nullptr, &_pTextureRV, 0);
	}
	else
	{
		hr = CreateWICTextureFromFile(_pd3dDevice, filePath, nullptr, &_pTextureRV, 0);
	}

}
