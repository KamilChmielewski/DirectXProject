#include "GameObjectManager.h"

GameObjectManager::GameObjectManager(ID3D11Device * _pd3dDevice, ID3D11VertexShader * _pVertexShader, ID3D11PixelShader * _pPixelShader)
{
	//Create Models
	_shipModel	 = new Model(_pd3dDevice);
	_sphereModel = new Model(_pd3dDevice);
	_groundModel = new Model(_pd3dDevice);
	//Load Models
	_shipModel->LoadModel("OBJ'S/car.obj");
	_sphereModel->LoadModel("Sphere.obj");
	_groundModel->LoadModel("OBJ'S/flatPlane.obj");
	//Create GameObjects
	_rootObject		= new GameObject(XMFLOAT3(0.0f, 0.0f, 0.0f));
	_shipObject		= new GameObject(XMFLOAT3(0.0f, 0.0f, 5.0f), _shipModel, _pd3dDevice, _pVertexShader, _pPixelShader);
	_sphereObject	= new GameObject(XMFLOAT3(5.0f, 0.2f, 10.0f), _sphereModel, _pd3dDevice, _pVertexShader, _pPixelShader);
	_groundObject	= new GameObject(XMFLOAT3(0.0f, -0.5f, 0.0f), _groundModel, _pd3dDevice, _pVertexShader, _pPixelShader);
	//SamplerStates
	_shipObject->CreateAndSetSamplerState();
	_sphereObject->CreateAndSetSamplerState();
	//ObjectTextures
	_shipObject->CreateAndSetTextureWIC(L"OBJ'S/Image_earth.jpg");
	_sphereObject->CreateAndSetTextureWIC(L"OBJ'S/Ball.jpg");
	_groundObject->CreateAndSetTextureWIC(L"OBJ'S/mountinTex.jpg");

	rootNode					= makeNode(_rootObject);
	rootNode->child				= makeNode(_shipObject);
	rootNode->sibling			= makeNode(_sphereObject);
	rootNode->sibling->sibling  = makeNode(_groundObject);
}

GameObjectManager::~GameObjectManager()
{

}

Node * GameObjectManager::makeNode(GameObject * data)
{
	Node* newNode;
	newNode = new Node();
	newNode->data = data;
	newNode->child = nullptr;
	newNode->sibling = nullptr;
	return newNode;
}

void GameObjectManager::Update()
{

}

void GameObjectManager::DrawObjects(ID3D11Buffer * _pConstantBuffer, ID3D11DeviceContext * _pImmediateContext, ConstantBuffer & cb)
{
	preOrderTraverse(rootNode, XMMatrixIdentity(), _pConstantBuffer, _pImmediateContext, cb);
}

void GameObjectManager::preOrderTraverse(Node * object, XMMATRIX & matrix, ID3D11Buffer * _pConstantBuffer, ID3D11DeviceContext * _pImmediateContext, ConstantBuffer & cb)
{
	//Matrix multiplication
	if (object->sibling != nullptr)
	{
		preOrderTraverse(object->sibling, matrix, _pConstantBuffer, _pImmediateContext, cb);
	}

	XMMATRIX result = object->data->GetMatrix() * matrix;

	if (object->child != nullptr)
	{
		preOrderTraverse(object->child, result, _pConstantBuffer, _pImmediateContext, cb);
	}

	if (object->data)
	{
		object->data->Draw(_pConstantBuffer, _pImmediateContext, cb, result);
	}
}
