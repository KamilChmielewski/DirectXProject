//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    DirectionalLight DL;
    Material Mtrl;
    float3 gEyePosW;
    float Padding;
}

cbuffer cbPerFrame : register(b1)
{
    float gFogStart;
    float gFogRange;
    float gFogColor;
}

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

void ComputeDirectionalLight(Material mat, DirectionalLight L,
                             float3 normal, float3 toEye,
					         out float4 ambient,
						     out float4 diffuse,
						     out float4 spec)
{
	// Initialize outputs.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -L.Direction;

	// Add ambient term.
    ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
    float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec = specFactor * mat.Specular * L.Specular;
	}
}

struct VS_IN
{
    float3 PosL		: POSITION;
    float3 NormalL	: NORMAL;
	float2 Tex		: TEXCOORD;

};
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 PosH		: SV_POSITION;
    float3 PosW		: POSITION;
	float3 NormalW	: NORMAL;
	float2 Tex		: TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_IN vin)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.PosW = mul(float4(vin.PosL, 1.0f), World).xyz;
	output.PosH = mul(float4(output.PosW, 1.0f), View);
	output.PosH = mul(output.PosH, Projection);

	output.NormalW = mul(float4(vin.NormalL, 0.0f), World).xyz; 

	output.Tex = vin.Tex;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT output ) : SV_Target
{
    // Interpolating normal can unnormalize it, so normalize it.
    output.NormalW = normalize(output.NormalW);

	// The toEye vector is used in lighting.
    float3 toEye = gEyePosW - output.PosW;

	// Cache the distance to the eye from this surface point.
    float distToEye = length(toEye);

	// Normalize.
    toEye /= distToEye;
	
	//
	// Texturing
	//
	//Defualt to multiplicative identity
	float4 texColor = float4(1, 1, 1, 1);
	texColor = txDiffuse.Sample(samLinear, output.Tex);
    clip(texColor.a - 0.1f);

	float4 litColor = texColor;

	//
	// Lighting.
	//

	// Start with a sum of zero. 
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.  
        float4 A, D, S; 
		A = float4(0.0f, 0.0f, 0.0f, 0.0f);
		D = float4(0.0f, 0.0f, 0.0f, 0.0f);
		S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    ComputeDirectionalLight(Mtrl, DL, output.NormalW, toEye, ambient, diffuse, spec);

        ambient += A;
        diffuse += D;
        spec += S;
    
		litColor = texColor*(ambient + diffuse) + spec;

        float fogLerp = saturate((distToEye - 15.0f) / 175.0f);
        litColor = lerp(litColor, gFogColor, fogLerp);
   
	    litColor.a = Mtrl.Diffuse.a * texColor.a;

    return litColor;
}

