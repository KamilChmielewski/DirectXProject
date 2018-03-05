#pragma once
//Load the Model and Draw the model
#include <d3d11.h>
#include "ModelLoader.h"
#include "Struct.h"
#include <DirectXMath.h>

using namespace DirectX;

class Model
{
private:
	Mesh* mesh;
	ID3D11Device* _pd3dDevice;

public:
	Model(ID3D11Device* _pd3dDevice);
	~Model();
	void LoadModel(char* path);
	void Draw(ID3D11DeviceContext* _pImmediateContext);
};