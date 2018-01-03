cbuffer CB0
{
	matrix WVPMatrix; //64 byte;
	matrix WorldViewMatrix;
	float4 AmbientLightColour; // 16
	float4 DirectionalLightColour;
	float4 DirectionalLightVector;
	float4 PointLightPosition;
	float4 PointLightColour;
};

TextureCube cube0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 texcoord : TEXCOORD;
};

VOut ModelVS(float4 position : POSITION, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = { 1, 1, 1, 1 };

	output.position = mul(WVPMatrix, position);

	output.texcoord = position.xyz;


	output.color = default_color;

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return color * cube0.Sample(sampler0, texcoord);
}