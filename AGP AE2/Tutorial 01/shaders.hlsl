Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
};

cbuffer CBuffer0
{
	matrix WVPMatrix; // 64 bytes
	float4 directional_light_vector; // 16
	float4 directional_light_colour; // 16
	float4 directional_light_vector2; // 16
	float4 directional_light_colour2; // 16
	float4 ambient_light_colour;  // 16
	float4 point_light_position;
	float4 point_light_colour;
	
};


VOut VShader(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;
	
	output.position = mul(WVPMatrix, position);

	//directional light calc
	float diffuse_amount = dot(directional_light_vector, normal);

	diffuse_amount = saturate(diffuse_amount);

	float diffuse_amount2 = dot(directional_light_vector2, normal);

	diffuse_amount2 = saturate(diffuse_amount2);

	//point light calc
	float4 lightvector = point_light_position - position;
	float point_amount = dot(normalize(lightvector), normal);

	point_amount = saturate(point_amount);
	
	//calculating the colours that are lit on the object
	output.color = ambient_light_colour + ((directional_light_colour * diffuse_amount) + (directional_light_colour2 * diffuse_amount2)) + (point_light_colour * point_amount);

	output.texcoord = texcoord;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}