cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	matrix WorldInverseTranspose;
	float4 vecEye;
	float4 wire;
	float4 color;
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 bary : COLOR0;
	float3 normal : NORMAL0;
};

struct VertexShaderOuput
{
	float4 pos : SV_POSITION;
	float4 view : TEXCOORD0;
	float4 color: COLOR0;
	float3 norm : NORMAL0;
	float3 bary : NORMAL1;
	float4 wire : NORMAL2;
};

VertexShaderOuput main(VertexShaderInput input)
{
	VertexShaderOuput output;
	output.wire = wire;

	float4 pos = float4(input.pos, 1.0f);
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);

	output.pos = pos;
	output.color = color;
	output.bary = input.bary;
	output.norm = normalize(mul(WorldInverseTranspose, input.normal));
	output.view = normalize(pos - vecEye);

	return output;
}
