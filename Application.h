#pragma once

#include <windows.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "GameObjectManager.h"
#include "Camera2.h"
#include "CameraTest.h"
#include "Camera3.h"
#include "Model.h"
#include "Struct.h"
#include <wincon.h>

using namespace DirectX;


// this need to be changed with the input layout

class Application
{
private:
	//Microsoft functions
	POINT mLastMousePos;

	bool bool_Key_P;
	bool wireframe_State;
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;

	//Depth/Stencil Buffer
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;

	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	ID3D11Buffer*           _pfogBuffer;

	XMFLOAT4X4              _world;
	XMFLOAT4X4              _world2;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;

	//RenderStates
	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _noCull;

	//Camera
	Camera2 _camera2;
	Camera3* _camera3;
	CameraTest* _cameraTest;

	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float mouseSpeed = 0.0005f;
	
	//lighting
	DirectionalLight DL;
	MaterialStruct Mtrl;
	XMFLOAT3 mEyePosW;

	//ObjTest* objObject;

	float x = 0;
	float y = 0;
	float z = 0;

	GameObjectManager* _gameObjectManager;
	GameObject* _arenaCage;
	Model* _arenaCageModel;

	//List of all the render Items that I will want to use
//	std::vector<std::unique_ptr<GameObject2>> mAllRObjects;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	void OnMouseMove(WPARAM btnState, int x, int y);

	UINT _WindowHeight;
	UINT _WindowWidth;

	//cube class
	//Cube* cube;
public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

