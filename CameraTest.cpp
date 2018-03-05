#include "CameraTest.h"

CameraTest::CameraTest(UINT WindowWidth, UINT WindowHeight, XMVECTOR EYE, XMVECTOR AT, XMVECTOR UP)
{
	this->WindowWidth = WindowWidth;
	this->WindowHeight = WindowHeight;

	_eye = XMFLOAT3(0.0f, 0.0f, -3.0f);
	_at = XMFLOAT3(0.0f, 0.0f, 0.0f);
	_up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	XMStoreFloat4x4(&proj, XMMatrixIdentity());
	XMStoreFloat4x4(&view, XMMatrixIdentity());

	XMStoreFloat4x4(&view, XMMatrixLookAtLH(XMLoadFloat3(&_eye), XMLoadFloat3(&_at), XMLoadFloat3(&_up)));
	XMStoreFloat4x4(&proj, XMMatrixPerspectiveFovLH(70.0f, WindowWidth / (FLOAT)WindowHeight, 0.01f, 1000.0f));
	
	//XMStoreFloat4x4(&view, XMMatrixLookAtLH(EYE, AT, UP));
	//XMStoreFloat4x4(&proj, XMMatrixPerspectiveFovLH(70.0f, WindowWidth / (FLOAT)WindowHeight, 0.01f, 100.0f));
	
	position = XMLoadFloat3(&_eye);
}

CameraTest::~CameraTest()
{

}

void CameraTest::SetPosition(XMFLOAT3 & position)
{
	this->position = XMLoadFloat3(&_eye);
}

void CameraTest::UpdateMatrix(float t)
{
	static float time = t;
	float lastTime = time;
	float deltaTime = time - lastTime;
	time = lastTime;
	time = t;

	static bool initCursor = false;

	if ((GetKeyState(VK_SHIFT) & 0x8000))
	{
		if (!initCursor)
		{
			SetCursorPos(WindowWidth / 2, WindowHeight / 2);
			initCursor = true;
		}

		GetCursorPos(&mousePos);
		SetCursorPos(WindowWidth / 2, WindowHeight / 2);

		//Compute new orientation
		horizontalAngle += mouseSpeed *  float((float)WindowWidth / 2.0f - mousePos.x);
		verticalAngle -= mouseSpeed *  float((float)WindowHeight / 2.0f - mousePos.y);


		XMFLOAT3 direction(cos(verticalAngle)* sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
		XMVECTOR d = XMLoadFloat3(&direction);

		XMFLOAT3 right(sin(horizontalAngle - 3.14f / 2.0f), 0.0f, cos(horizontalAngle - 3.14f / 2.0f));
		XMVECTOR r = XMLoadFloat3(&right);

		XMVECTOR up = XMVector3Cross(r, d);

		if ((GetAsyncKeyState('W') & 0x8000))
		{
			position += (d * 0.0005 * speed);
		}
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			position -= (d * 0.05 * speed);
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			position += (r * deltaTime * speed);
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			position -= (r * deltaTime * speed);
		}
		
		XMStoreFloat4x4(&view, XMMatrixLookAtLH(position, position + d, up));
	//	XMStoreFloat4x4(&proj, XMMatrixPerspectiveFovLH(70.0f, WindowWidth / (FLOAT)WindowHeight, 0.01f, 100.0f));

	}
	else if (!(GetKeyState(VK_SHIFT) & 0x8000))
	{
		initCursor = false;
	}
}

XMMATRIX CameraTest::GetView()
{
	return XMLoadFloat4x4(&view);
}

XMMATRIX CameraTest::GetProj()
{
	return XMLoadFloat4x4(&proj);
}

XMFLOAT4X4 CameraTest::Get4X4View()
{
	return view;
}

XMFLOAT4X4 CameraTest::Get4X4Proj()
{
	return proj;
}
