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

	float3 world_view_pos = mul(WorldViewMatrix, position);

	float3 world_view_normal = mul(WorldViewMatrix, normal);
	world_view_normal = normalize(world_view_normal);

	float3 eyer = -normalize(world_view_pos);

	output.texcoord = 2.0f * dot(eyer, world_view_normal) * world_view_normal - eyer;

	output.color = default_color;

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return color * cube0.Sample(sampler0, texcoord);
}