#include "GameObject.h"

GameObject::GameObject(XMFLOAT3& _position, Model* model, ID3D11Device* _pd3dDevice, ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader)
{
	this->_position = _position;
	this->_pd3dDevice = _pd3dDevice;
	this->_pVertexShader = _pVertexShader;
	this->_pPixelShader = _pPixelShader;
	this->model = model;

	XMStoreFloat4x4(&_world, XMMatrixIdentity());

	MaterialStruct Mtrl;

	Mtrl.Ambient = XMFLOAT4(0.0f, 1.0f, 0.2f, 1.0f);
	Mtrl.Diffuse = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	Mtrl.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	material = new Material(Mtrl, this->_pVertexShader, this->_pPixelShader);
}

GameObject::GameObject(XMFLOAT3& _position)
{
	this->_position = _position;
	model = nullptr;
}

GameObject::~GameObject()
{

}

void GameObject::CreateAndSetSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

}

void GameObject::ModelToLoad(Model * model)
{
	this->model = model;
}

void GameObject::CreateAndSetTextureWIC(const wchar_t* filePath)
{
	HRESULT hr = CreateWICTextureFromFile(_pd3dDevice, filePath, nullptr, &_pTextureRV, 0);
}

void GameObject::CreateAndSetTextureDDS(const wchar_t * filePath)
{
	HRESULT hr = CreateDDSTextureFromFile(_pd3dDevice, filePath, nullptr, &_pTextureRV, 0);
}

void GameObject::Update(float t)
{
	XMStoreFloat4x4(&_world, (XMMatrixRotationX(t) * XMMatrixTranslation(0.0f, 1.0f, 25.0f)));
}

void GameObject::Draw(ID3D11Buffer* _pConstantBuffer, ID3D11DeviceContext* _pImmediateContext, ConstantBuffer& cb, XMMATRIX result)
{
	if (model == nullptr)
	{
		return;
	}
		XMMATRIX world = XMLoadFloat4x4(&_world);

		cb.mWorld = XMMatrixTranspose(result);

		material->Draw(_pImmediateContext, cb);
		_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);

		model->Draw(_pImmediateContext);
}

//enum Type  { Type0, Type1 };
//
//void something(Type type)
//{
//	array[type];
//
//	psSetshader(Array[type]);
//}