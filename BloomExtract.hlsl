// Bloom shader 1

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer BloomExtractBuffer : register(b0)
{
	float BloomThreshold;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{


	float4 color = shaderTexture.Sample(SampleType, input.tex);
	return saturate((color - BloomThreshold) / (1 - BloomThreshold));
	
}

