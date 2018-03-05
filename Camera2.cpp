#include "Camera2.h"

Camera2::Camera2()
{
	SetLens(0.25*XM_PI, 1.0f, 1.0f, 1000.0f);
}

Camera2::~Camera2()
{

}

XMFLOAT3 Camera2::GetPosition3f()
{
	return _position;
}

void Camera2::SetPosition(XMFLOAT3 & _position)
{
	this->_position = _position;
	_viewDirty = true;
}

void Camera2::SetLens(float fovY, float aspect, float zn, float zf)
{
	//Cache properties
	_fovY = fovY;
	_aspect = aspect;
	_nearZ = zn;
	_farZ = zf;

	_nearWindowHeight = 2.0f * _nearZ * tanf(0.5*_fovY);
	_farWindowHeight = 2.0f *_farZ * tanf(0.5 * _fovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);
	XMStoreFloat4x4(&_proj, P);
}

XMMATRIX Camera2::GetView() const
{
	assert(!_viewDirty);
	return XMLoadFloat4x4(&_view);
}

XMMATRIX Camera2::GetProj() const
{
	return XMLoadFloat4x4(&_proj);
}

void Camera2::Strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&_right);
	XMVECTOR p = XMLoadFloat3(&_position);
	XMStoreFloat3(&_position, XMVectorMultiplyAdd(s, r, p));

	_viewDirty = true;
}

void Camera2::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&_look);
	XMVECTOR p = XMLoadFloat3(&_position);
	XMStoreFloat3(&_position, XMVectorMultiplyAdd(s, l, p));

	_viewDirty = true;
}

void Camera2::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&_right), angle);

	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), R));
	XMStoreFloat3(&_look, XMVector3TransformNormal(XMLoadFloat3(&_look), R));

	_viewDirty = true;
}

void Camera2::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), R));
	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), R));
	XMStoreFloat3(&_look, XMVector3TransformNormal(XMLoadFloat3(&_look), R));

	_viewDirty = true;
}

void Camera2::UpdateViewMatrix()
{
	if (_viewDirty)
	{
		XMVECTOR R = XMLoadFloat3(&_right);
		XMVECTOR U = XMLoadFloat3(&_up);
		XMVECTOR L = XMLoadFloat3(&_look);
		XMVECTOR P = XMLoadFloat3(&_position);

		// Keep camera's axes orthogonal to each other and of unit length.
		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		// U, L already ortho-normal, so no need to normalize cross product.
		R = XMVector3Cross(U, L);

		// Fill in the view matrix entries.
		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&_right, R);
		XMStoreFloat3(&_up, U);
		XMStoreFloat3(&_look, L);

		_view(0, 0) = _right.x;
		_view(1, 0) = _right.y;
		_view(2, 0) = _right.z;
		_view(3, 0) = x;

		_view(0, 1) = _up.x;
		_view(1, 1) = _up.y;
		_view(2, 1) = _up.z;
		_view(3, 1) = y;

		_view(0, 2) = _look.x;
		_view(1, 2) = _look.y;
		_view(2, 2) = _look.z;
		_view(3, 2) = z;

		_view(0, 3) = 0.0f;
		_view(1, 3) = 0.0f;
		_view(2, 3) = 0.0f;
		_view(3, 3) = 1.0f;

		_viewDirty = false;
	}
}
