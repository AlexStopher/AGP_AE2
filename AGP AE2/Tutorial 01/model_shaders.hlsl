cbuffer CB0
{
	matrix WVPMatrix; //64 byte;
	float4 AmbientLightColour; // 16
	float4 DirectionalLightColour;
	float4 DirectionalLightVector;
};

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_color = { 1, 1, 1, 1 };

	output.position = mul(WVPMatrix, position);

	output.texcoord = texcoord;

	float4 diffuse_amount = dot(DirectionalLightVector, normal);

	diffuse_amount = saturate(diffuse_amount);

	output.color = AmbientLightColour + (DirectionalLightColour * diffuse_amount);

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}