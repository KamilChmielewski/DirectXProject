#pragma once
#include <DirectXMath.h>
#include "Model.h"
#include "Struct.h"
#include "Material.h"
//Texture loading
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

class GameObject
{
private:
	Model* model;
	Material* material;
	XMFLOAT3 _position;
	XMFLOAT4X4 _world;
	ID3D11Device*			_pd3dDevice;

	ID3D11VertexShader* _pVertexShader;
	ID3D11PixelShader*  _pPixelShader;

	ID3D11ShaderResourceView*	_pTextureRV = nullptr;
	ID3D11SamplerState*			_pSamplerLinear = nullptr;

public:
	GameObject(XMFLOAT3& _position, Model* model, ID3D11Device* _pd3dDevice, ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader);
	GameObject(XMFLOAT3& _position);
	~GameObject();
	XMMATRIX GetMatrix() { return XMMatrixIdentity() * XMMatrixTranslation(_position.x, _position.y, _position.z); }
	void CreateAndSetSamplerState();
	void CreateAndSetTextureWIC(const wchar_t* filePath);
	void CreateAndSetTextureDDS(const wchar_t* filePath);
	void Update(float t);
	void Draw(ID3D11Buffer* _pConstantBuffer, ID3D11DeviceContext* _pImmediateContext, ConstantBuffer& cb, XMMATRIX result);

	void ModelToLoad(Model* model);
};