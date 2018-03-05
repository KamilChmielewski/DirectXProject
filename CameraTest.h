#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <ostream>
#include <sstream>
#include <iostream>

using namespace DirectX;

class CameraTest
{
private:
	POINT mousePos;
	XMFLOAT4X4 view;
	XMFLOAT4X4 proj;
	XMFLOAT3 _eye;
	XMFLOAT3 _at;
	XMFLOAT3 _up;
	XMVECTOR position;
	UINT WindowWidth;
	UINT WindowHeight;
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float mouseSpeed = 0.05f;
	float initialFoV = 70.0f;
	float speed = 3.0f;
	
	
public:
	CameraTest(UINT WindowWidth, UINT WindowHeight, XMVECTOR EYE, XMVECTOR AT, XMVECTOR UP);
	~CameraTest();
	void SetPosition(XMFLOAT3& position);
	void UpdateMatrix(float t);
	XMMATRIX GetView();
	XMMATRIX GetProj();
	XMFLOAT4X4 Get4X4View();
	XMFLOAT4X4 Get4X4Proj();
};