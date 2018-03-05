#pragma once
#include <directxmath.h>
#include <vector>
#include <iostream>
#include <windows.h>
#include <d3dcompiler.h>
#include <d3d11_1.h>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT3 Normal;
};

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
};

struct Textures
{
	float u, v;
};

struct Mesh
{
	std::vector<Vertex> elements;
	std::vector<unsigned short> Indices;
	unsigned short vertexIndex[3], uvIndex[3], normalIndex[3];
	std::vector<unsigned short> vertexIndices, uvIndices, normalIndices;
	std::vector<XMFLOAT3> indexed_vertices;
	std::vector<XMFLOAT2> indexed_uvs;
	std::vector<XMFLOAT3> indexed_normals;

	ID3D11Buffer* _pVertexBuffer;
	ID3D11Buffer* _pIndexBuffer;
};

struct PackedVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 UV;
	//Overloading < function
	bool operator <(const PackedVertex that) const { return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0; } //So if that is greater than this then return true
};

struct DirectionalLight
{
//	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad; // Not sure if I need this but I can now have an array of lights if I wanted to?
};

struct MaterialStruct
{
//	Material() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Reflect;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	DirectionalLight DL;
	MaterialStruct Mtrl;
	XMFLOAT3 gEyePosW;
};

struct cbPerFrame
{
	float gFogStart;
	float gFogRange;
	XMFLOAT4 gFogColor;
};

enum class RenderLayer : int
{
	Opaque = 0,
	AlphaTested,
	Transparent,
	Default
};

