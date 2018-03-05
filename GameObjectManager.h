#pragma once
#include "Struct.h"
#include <DirectXMath.h>
#include "GameObject.h"
#include "Model.h"

using namespace DirectX;

struct Node
{
	GameObject* data;
	Node* child;
	Node* sibling;
};

class GameObjectManager
{
private:
	Node* makeNode(GameObject* data);
	//Models that are going to be used
	Model* _shipModel;
	Model* _sphereModel;
	Model* _groundModel;
	//Objects that are going to be used
	GameObject* _rootObject;
	GameObject* _shipObject;
	GameObject* _sphereObject;
	GameObject* _groundObject;

	Node* rootNode;

public:
	GameObjectManager(ID3D11Device* _pd3dDevice, ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader);
	~GameObjectManager();
	void Update();
	void DrawObjects(ID3D11Buffer* _pConstantBuffer, ID3D11DeviceContext* _pImmediateContext, ConstantBuffer& cb);
	void preOrderTraverse(Node* object, XMMATRIX& matrix, ID3D11Buffer* _pConstantBuffer, ID3D11DeviceContext* _pImmediateContext, ConstantBuffer& cb);

};