cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	matrix WorldInverseTranspose;
	float4 vecEye;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

struct VertexShaderOuput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

VertexShaderOuput main(VertexShaderInput input)
{
	VertexShaderOuput output;
	float4 pos = float4(input.pos, 1.0f);
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	output.pos = pos;
	output.color = input.color;

	return output;
}
