// Bloom shader 1


//S. (2016).stegu / webgl - noise.GitHub.https://github.com/stegu/webgl-noise



Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightPosition;
	float padding;
};
cbuffer WaterBuffer : register(b1)
{
	float time;
	float alpha;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 position3D : TEXCOORD2;
};

/*float fade(float t) {
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float3 grad(float3 p) {
	const float texture_width = 256.0;
	float4 v = shaderTexture.Sample(SampleType, float2((p.x + p.z) / texture_width, (p.y - p.z) / texture_width));
	return normalize(v.xyz * 2.0 - float3(1.0,1.0,1.0));
}

float noise(float3 p) {
	/* Calculate lattice points. */
	/*float3 p0 = floor(p);
	float3 p1 = p0 + float3(1.0, 0.0, 0.0);
	float3 p2 = p0 + float3(0.0, 1.0, 0.0);
	float3 p3 = p0 + float3(1.0, 1.0, 0.0);
	float3 p4 = p0 + float3(0.0, 0.0, 1.0);
	float3 p5 = p4 + float3(1.0, 0.0, 0.0);
	float3 p6 = p4 + float3(0.0, 1.0, 0.0);
	float3 p7 = p4 + float3(1.0, 1.0, 0.0);

	/* Look up gradients at lattice points. */
	/*float3 g0 = grad(p0);
	float3 g1 = grad(p1);
	float3 g2 = grad(p2);
	float3 g3 = grad(p3);
	float3 g4 = grad(p4);
	float3 g5 = grad(p5);
	float3 g6 = grad(p6);
	float3 g7 = grad(p7);

	float t0 = p.x - p0.x;
	float fade_t0 = fade(t0);

	float t1 = p.y - p0.y;
	float fade_t1 = fade(t1);

	float t2 = p.z - p0.z;
	float fade_t2 = fade(t2);

	float p0p1 = (1.0 - fade_t0) * dot(g0, (p - p0)) + fade_t0 * dot(g1, (p - p1));
	float p2p3 = (1.0 - fade_t0) * dot(g2, (p - p2)) + fade_t0 * dot(g3, (p - p3));

	float p4p5 = (1.0 - fade_t0) * dot(g4, (p - p4)) + fade_t0 * dot(g5, (p - p5));
	float p6p7 = (1.0 - fade_t0) * dot(g6, (p - p6)) + fade_t0 * dot(g7, (p - p7));

	float y1 = (1.0 - fade_t1) * p0p1 + fade_t1 * p2p3;
	float y2 = (1.0 - fade_t1) * p4p5 + fade_t1 * p6p7;

	return (1.0 - fade_t2) * y1 + fade_t2 * y2;
}*/

float3 mod289(float3 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 mod289(float4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 permute(float4 x) {
	return mod289(((x * 34.0) + 1.0) * x);
}

float4 taylorInvSqrt(float4 r)
{
	return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(float3 v)
{
	const float2  C = float2(1.0 / 6.0, 1.0 / 3.0);
	const float4  D = float4(0.0, 0.5, 1.0, 2.0);

	// First corner
	float3 i = floor(v + dot(v, C.yyy));
	float3 x0 = v - i + dot(i, C.xxx);

	// Other corners
	float3 g = step(x0.yzx, x0.xyz);
	float3 l = 1.0 - g;
	float3 i1 = min(g.xyz, l.zxy);
	float3 i2 = max(g.xyz, l.zxy);

	//   x0 = x0 - 0.0 + 0.0 * C.xxx;
	//   x1 = x0 - i1  + 1.0 * C.xxx;
	//   x2 = x0 - i2  + 2.0 * C.xxx;
	//   x3 = x0 - 1.0 + 3.0 * C.xxx;
	float3 x1 = x0 - i1 + C.xxx;
	float3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
	float3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

  // Permutations
	i = mod289(i);
	float4 p = permute(permute(permute(
		i.z + float4(0.0, i1.z, i2.z, 1.0))
		+ i.y + float4(0.0, i1.y, i2.y, 1.0))
		+ i.x + float4(0.0, i1.x, i2.x, 1.0));

	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
	float n_ = 0.142857142857; // 1.0/7.0
	float3  ns = n_ * D.wyz - D.xzx;

	float4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

	float4 x_ = floor(j * ns.z);
	float4 y_ = floor(j - 7.0 * x_);    // mod(j,N)

	float4 x = x_ * ns.x + ns.yyyy;
	float4 y = y_ * ns.x + ns.yyyy;
	float4 h = 1.0 - abs(x) - abs(y);

	float4 b0 = float4(x.xy, y.xy);
	float4 b1 = float4(x.zw, y.zw);

	//float4 s0 = float4(lessThan(b0,0.0))*2.0 - 1.0;
	//float4 s1 = float4(lessThan(b1,0.0))*2.0 - 1.0;
	float4 s0 = floor(b0) * 2.0 + 1.0;
	float4 s1 = floor(b1) * 2.0 + 1.0;
	float4 sh = -step(h, float4(0.0,0.0,0.0,0.0));

	float4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	float4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	float3 p0 = float3(a0.xy, h.x);
	float3 p1 = float3(a0.zw, h.y);
	float3 p2 = float3(a1.xy, h.z);
	float3 p3 = float3(a1.zw, h.w);

	//Normalise gradients
	float4 norm = taylorInvSqrt(float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	// Mix final noise value
	float4 m = max(0.5 - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
	m = m * m;
	return 105.0 * dot(m * m, float4(dot(p0, x0), dot(p1, x1),
		dot(p2, x2), dot(p3, x3)));

}

float4 main(InputType input) : SV_TARGET
{
	
	float4	textureColor;
	float3	lightDir;
	float	lightIntensity;
	float4	color;

	// Invert the light direction for calculations.
	lightDir = normalize(input.position3D - lightPosition);

	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, -lightDir));

	// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = ambientColor + (diffuseColor * lightIntensity); //adding ambient
	color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	color = color* textureColor;

	//float3 v = float3(2.0 * input.position3D.xyz * (2.0 + sin(0.5 * time)));
	

	float x = (input.position3D.x /5 +time*0.2) * 2 * 3.14;
	float z = (input.position3D.z /5 +time*0.2) * 2 * 3.14;

	float offset = 0.5 * 0.5 * (sin(z) + cos(x));

	float3 v = input.position3D + float3(offset,offset,offset);
	
	float n = snoise(v);
	
	color = color * float4(0.5 + 0.5 * float3(n, n, n), alpha);
	return color;

}
