// Skybox pixel shader

TextureCube	EnvironmentMap : register(t0);
SamplerState SampleType : register(s0);

cbuffer CamBuffer : register(b0)
{
	float3 cpos;
	
};
struct InputType
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD0;
    //float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
	float3 v = normalize(input.position.xyz - cpos);
	float4 color = EnvironmentMap.Sample(SampleType, -v);
    return color;
}

