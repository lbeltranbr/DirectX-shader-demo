Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

#define SAMPLE_COUNT 15

cbuffer GaussianBuffer : register(b0)
{
	float2 SampleOffsets[SAMPLE_COUNT];
	//float SampleWeights[SAMPLE_COUNT];
	float padding[2];

};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{

	float4 color=0;

	/*for (int i = 0; i < SAMPLE_COUNT;i++) {
		color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[i]) * SampleWeights[i];
	}*/
	
		
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[0])*0.106 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[1])*0.102 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[2])*0.102 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[3])*0.093 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[4])*0.093 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[5])*0.08 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[6])*0.08 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[7])*0.064  ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[8])*0.064 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[9])*0.048 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[10])*0.048;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[11])*0.034 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[12])*0.034;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[13])*0.022 ;
	color += shaderTexture.Sample(SampleType, input.tex + SampleOffsets[14])*0.022 ;
	
	return color;

}

