#pragma once
#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//IdentityMatrix
static XMFLOAT4X4 Identity4x4()
{
	static DirectX::XMFLOAT4X4 I(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return I;
}

class Camera2
{
private:
	XMFLOAT3 _position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 _right = { 1.0f, 0.0f, 0.0f };
	XMFLOAT3 _up = { 0.0f, 0.1f, 0.0f };
	XMFLOAT3 _look = { 0.0f, 0.0f, 1.0f };

	// Cache frustum properties
	float _nearZ = 0.0f;
	float _farZ = 0.0f;
	float _aspect = 0.0f;
	float _fovY = 0.0f;
	float _nearWindowHeight = 0.0f;
	float _farWindowHeight = 0.0f;

	bool _viewDirty = true;

	//View/Proj matrices.

	XMFLOAT4X4 _view = Identity4x4();
	XMFLOAT4X4 _proj = Identity4x4();

public:
	Camera2();
	~Camera2();

	void SetPosition(XMFLOAT3& _position);
	void SetLens(float fovY, float aspect, float zn, float zf);
	XMFLOAT3 GetPosition3f();

	XMMATRIX GetView()const;
	XMMATRIX GetProj()const;

	//Strafe/Walk
	void Strafe(float d); //D for distance
	void Walk(float d);

	//Rotate the Camera
	void Pitch(float angle);
	void RotateY(float angle);

	//After modifiying the cameras position and orientation, call to rebuild the viewMatrix
	void UpdateViewMatrix();

};