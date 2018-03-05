#include "Application.h"
#include <string>
#include <iostream>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
 }

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
	if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
		return E_FAIL;
	}

	RECT rc;
	GetClientRect(_hWnd, &rc);
	_WindowWidth = rc.right - rc.left;
	_WindowHeight = rc.bottom - rc.top;

	if (FAILED(InitDevice()))
	{
		Cleanup();

		return E_FAIL;
	}

	// Initialize the world matrix
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	//object 2
	XMStoreFloat4x4(&_world2, XMMatrixIdentity());

	_gameObjectManager  = new GameObjectManager(_pd3dDevice, _pVertexShader, _pPixelShader);

	_arenaCageModel = new Model(_pd3dDevice);
	_arenaCageModel->LoadModel("OBJ'S/cage.obj");

	_arenaCage = new GameObject(XMFLOAT3(0.0f, 0.0f, 0.0f), _arenaCageModel, _pd3dDevice, _pVertexShader, _pPixelShader);
	_arenaCage->CreateAndSetSamplerState();
	_arenaCage->CreateAndSetTextureDDS(L"OBJ'S/WireFence.dds");

	_camera2.SetPosition(XMFLOAT3(0.0f, 0.0f, -3.0f));
	//_cameraTest.SetPosition(XMFLOAT3(0.0f, 0.0f, -3.0f));
	

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMFLOAT3 eye = XMFLOAT3(0.0f, 0.0f, -3.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_cameraTest = new CameraTest(_WindowWidth, _WindowHeight, Eye, At, Up);

	_camera3 = new Camera3(eye, at, up, (float)_WindowWidth, (float)_WindowHeight, 0.01f, 200.0f);

//	XMStoreFloat4x4(&_cameraTest.Get4X4View(), XMMatrixLookAtLH(Eye, At, Up));

	// Initialize the projection matrix
//	XMStoreFloat4x4(&_cameraTest.Get4X4Proj(), XMMatrixPerspectiveFovLH(70.0f, _WindowWidth / (FLOAT)_WindowHeight, 0.01f, 100.0f));

	DL.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	DL.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	DL.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	DL.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	Mtrl.Ambient = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	Mtrl.Diffuse = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	Mtrl.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

//	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);
	
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Define the input layout

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{									   //This data 0,0 hows where the vertex starts
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//The data 0, 12 shows from which point to draw the colours
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
		return hr;

	// Set the input layout
	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;


	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	_hInst = hInstance;
	RECT rc = { 0, 0, 1024, 768 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	_hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!_hWnd)
		return E_FAIL;

	ShowWindow(_hWnd, nCmdShow);

	return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		if (pErrorBlob) pErrorBlob->Release();

		return hr;
	}

	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

HRESULT Application::InitDevice()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = _WindowWidth;
	sd.BufferDesc.Height = _WindowHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	//Create DepthBuffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (FAILED(hr))
		return hr;

	hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
	pBackBuffer->Release();

	if (FAILED(hr))
		return hr;

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);


	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)_WindowWidth;
	vp.Height = (FLOAT)_WindowHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	_pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();

	InitIndexBuffer();

	// Set index buffer

	// Set primitive topology
	_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	D3D11_BUFFER_DESC perFrame;
	ZeroMemory(&perFrame, sizeof(perFrame));
	perFrame.Usage = D3D11_USAGE_DEFAULT;
	perFrame.ByteWidth = sizeof(cbPerFrame);
	perFrame.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	perFrame.CPUAccessFlags = 0;
	hr = _pd3dDevice->CreateBuffer(&perFrame, nullptr, &_pfogBuffer);

	//Rasterise
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	wfdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;

	hr = _pd3dDevice->CreateRasterizerState(&rastDesc, &_noCull);



	bool_Key_P = false;
	wireframe_State = false;

	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Application::Cleanup()
{
	if (_pImmediateContext) _pImmediateContext->ClearState();

	if (_pConstantBuffer) _pConstantBuffer->Release();
	if (_pfogBuffer) _pfogBuffer->Release();
	if (_pVertexBuffer) _pVertexBuffer->Release();
	if (_pIndexBuffer) _pIndexBuffer->Release();
	if (_pVertexLayout) _pVertexLayout->Release();
	if (_pVertexShader) _pVertexShader->Release();
	if (_pPixelShader) _pPixelShader->Release();
	if (_pRenderTargetView) _pRenderTargetView->Release();
	if (_pSwapChain) _pSwapChain->Release();
	if (_pImmediateContext) _pImmediateContext->Release();
	if (_pd3dDevice) _pd3dDevice->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_noCull) _noCull->Release();

	//depth and stencil buffer
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

}

void Application::Update()
 {
		// Update our time
		static float t = 0.0f;
	
		if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
		 {
		t += (float)XM_PI * 0.0125f;
		}
	else
		 {
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		
			if (dwTimeStart == 0)
			 dwTimeStart = dwTimeCur;
		
			t = (dwTimeCur - dwTimeStart) / 1000.0f;
		}
	
			//camera->Update(t);
	
	
		if ((GetKeyState('P') & 0x8000) && !bool_Key_P)
		 {
		bool_Key_P = true;
		wireframe_State = !wireframe_State;
		 }
	
		else if (!(GetKeyState('P') & 0x8000) && bool_Key_P)
		 {
		bool_Key_P = false;
		 }
	
		if (wireframe_State == false)
		 {
			_pImmediateContext->RSSetState(nullptr);
		 }
	 else if (wireframe_State == true)
		 {
			_pImmediateContext->RSSetState(_wireFrame);
		 }

		//bool initCursor = false;
		//
	
		//if ((GetKeyState(VK_LCONTROL) & 0x8000))
		// {
		//// Make each pixel correspond to a quarter of a degree.
		//GetCursorPos(&mLastMousePos); 
		//SetCursorPos(_WindowWidth/2, _WindowHeight/2);
	
		//horizontalAngle += mouseSpeed * float(1024 / 2 - mLastMousePos.x);
		//verticalAngle -= mouseSpeed * float(768 / 2 - mLastMousePos.y);
		//
		//_camera2.Pitch(horizontalAngle);
		//_camera2.RotateY(verticalAngle);
		//}
		//else if (!(GetKeyState(VK_SHIFT) & 0x8000))
		//{
		//	initCursor = false;
		//}
	//mLastMousePos.x = x;
	//mLastMousePos.y = y;


			//Camera movement
	/*	if (GetAsyncKeyState('W') & 0x8000)
		_camera2.Walk(0.003f*t);
	
		if (GetAsyncKeyState('S') & 0x8000)
		_camera2.Walk(-0.003f*t);
	
		if (GetAsyncKeyState('A') & 0x8000)
		 _camera2.Strafe(-0.003f*t);
	
		if (GetAsyncKeyState('D') & 0x8000)
		 _camera2.Strafe(0.003f*t);

		_camera2.UpdateViewMatrix();

		_cameraTest->UpdateMatrix(t);*/

		//_camera3->Update(t);
		_cameraTest->UpdateMatrix(t);
	}

void Application::Draw()
{
	//
	// Clear the back buffer
	//
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	_pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	cbPerFrame cbPF;
	cbPF.gFogStart = 15.0f;
	cbPF.gFogRange = 175.0f;
	cbPF.gFogColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	_pImmediateContext->VSSetConstantBuffers(1, 1, &_pfogBuffer);
	_pImmediateContext->PSSetConstantBuffers(1, 1, &_pfogBuffer);

	
	XMFLOAT4X4 viewAsFloats = _cameraTest->Get4X4View();
	XMFLOAT4X4 projectionAsFloats = _cameraTest->Get4X4Proj();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);
	
	//
	// Update variables
	//
	ConstantBuffer cb;
//	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);

	
	//light
	//Directional Light
	cb.DL.Ambient = DL.Ambient;
	cb.DL.Diffuse = DL.Diffuse;
	cb.DL.Direction = DL.Direction;
	cb.DL.Specular = DL.Specular;

	//Material
	cb.Mtrl.Ambient = Mtrl.Ambient;
	cb.Mtrl.Diffuse = Mtrl.Diffuse;
	cb.Mtrl.Specular = Mtrl.Specular;
	
	cb.gEyePosW = _camera3->GetPosition();

//	_pImmediateContext->UpdateSubresource(_pfogBuffer, 0, nullptr, &cbPF, 0, 0);

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	//
	// Renders a triangle
	//
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	//RasteriseObject2

	_gameObjectManager->DrawObjects(_pConstantBuffer, _pImmediateContext, cb);
	
	_pImmediateContext->RSSetState(_noCull);
	_arenaCage->Draw(_pConstantBuffer, _pImmediateContext, cb, XMMatrixIdentity());

	//_pImmediateContext->UpdateSubresource(_pfogBuffer, 1, nullptr, &cbPF, 0, 0);

	//
	// Present our back buffer to our front buffer
	//
	_pSwapChain->Present(0, 0);
}