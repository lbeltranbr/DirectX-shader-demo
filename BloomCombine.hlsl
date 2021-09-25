Texture2D BaseTexture : register(t0);
Texture2D BloomTexture : register(t1);
SamplerState SampleType : register(s0);

cbuffer CombineBuffer : register(b0)
{
	float BloomSaturation;
	float BaseSaturation;
	float BloomIntensity;
	float BaseIntensity;

	float padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

// Helper for modifying the saturation of a color.
float4 AdjustSaturation(float4 color, float saturation)
{
	// The constants 0.3, 0.59, and 0.11 are chosen because the
	// human eye is more sensitive to green light, and less to blue.
	float grey = dot(color.rgb, float3(0.3, 0.59, 0.11));

	return lerp(grey, color, saturation);
}

float4 main(InputType input) : SV_TARGET
{

	float4 base = BaseTexture.Sample(SampleType, input.tex);
	float4 bloom = BloomTexture.Sample(SampleType, input.tex);

	// Adjust color saturation and intensity.
	bloom = AdjustSaturation(bloom, BloomSaturation) * BloomIntensity;
	base = AdjustSaturation(base, BaseSaturation) * BaseIntensity;

	// Darken down the base image in areas where there is a lot of bloom,
	// to prevent things looking excessively burned-out.
	base *= (1 - saturate(bloom));

	// Combine the two images.
	return base + bloom;

}