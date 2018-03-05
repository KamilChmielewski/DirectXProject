#include "Camera3.h"

Camera3::Camera3(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: _eye(position), _at(at), _up(up), _windowWidth(windowWidth), _windowHeight(windowHeight), _nearDepth(nearDepth), _farDepth(farDepth)
{
	XMFLOAT4 Eye = XMFLOAT4(_eye.x, _eye.y, _eye.z, 1.0f);
	XMFLOAT4 At = XMFLOAT4(_at.x, _at.y, _at.z, 1.0f);
	XMFLOAT4 Up = XMFLOAT4(_up.x, _up.y, _up.z, 0.0f);
	

	XMVECTOR EyeVector = XMLoadFloat4(&Eye);
	XMVECTOR AtVector = XMLoadFloat4(&At);
	XMVECTOR UpVector = XMLoadFloat4(&Up);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(EyeVector, AtVector, UpVector));

	// Initialize the projection matrix
	//XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(0.25f * XM_PI, _windowWidth / _windowHeight, _nearDepth, _farDepth));
}

Camera3::~Camera3()
{
}

void Camera3::Update(float t)
{
	static float time = t;
	float lastTime = time;
	float deltaTime = time - lastTime;
	time = lastTime;
	time = t;

	static bool initCursor = false;

	POINT mousePos;

	XMFLOAT4 direction = XMFLOAT4(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle), 0.0f);
	XMVECTOR d = XMLoadFloat4(&direction);

	XMFLOAT4 right = XMFLOAT4(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f), 0.0f);
	XMVECTOR r = XMLoadFloat4(&right);

	XMVECTOR up = XMVector3Cross(r, d);

	XMFLOAT3 Eye = XMFLOAT3(_eye.x, _eye.y, _eye.z);

	XMVECTOR EyeVector = XMLoadFloat3(&Eye);

	if ((GetKeyState(VK_SHIFT) & 0x8000))
	{
		if (!initCursor)
		{
			SetCursorPos(_windowWidth / 2, _windowHeight / 2);
			initCursor = true;
		}

		GetCursorPos(&mousePos);
		SetCursorPos(_windowWidth / 2, _windowHeight / 2);
		// Compute new orientation
		horizontalAngle -= mouseSpeed * float(1024 / 2 - mousePos.x);
		verticalAngle -= mouseSpeed * float(768 / 2 - mousePos.y);

		if ((GetAsyncKeyState('W') & 0x8000))
		{
			EyeVector += (d * 0.005 * 6.0f);
		}
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			EyeVector -= (d * 0.05 * speed);
		}
		if (GetAsyncKeyState('D') & 0x8000)
		{
			EyeVector += (r * deltaTime * speed);
		}
		if (GetAsyncKeyState('A') & 0x8000)
		{
			EyeVector -= (r * deltaTime * speed);
		}
		XMStoreFloat4x4(&_view, XMMatrixLookAtLH(EyeVector, EyeVector + d, up));
	}
	else if (!(GetKeyState(VK_SHIFT) & 0x8000))
	{
		initCursor = false;
	}


	
	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(0.25f * XM_PI, _windowWidth / _windowHeight, _nearDepth, _farDepth));
}

XMFLOAT4X4 Camera3::GetViewProjection() const
{
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

	XMFLOAT4X4 viewProj;

	XMStoreFloat4x4(&viewProj, view * projection);

	return viewProj;
}

void Camera3::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}
