// Uniforms.
//
uniform matrix wvp	 :WORLDVIEWPROJECTION;
uniform matrix world :WORLD;

uniform float3 light_pos0;
uniform float3 light_color0;
uniform float4 light_attn0;

uniform float3 cam_pos;


// Textures.
//
Texture2D tex0;
SamplerState defss
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

// Vertex Shader.
//
struct VSInput
{
	float4 Position	:POSITION;
	float2 Texture0	:TEXCOORD0;
	float3 Normal	:NORMAL;
};

struct VSOutput
{
	float4 Position	:SV_POSITION;
	float2 Texture0	:TEXCOORD0;
	float3 Normal	:NORMAL0;
	float3 CamDir	:NORMAL1;
	float3 LPos0	:NORMAL2;
};

VSOutput MyVS( VSInput input )
{
	VSOutput output;

	// Position
	output.Position = mul(input.Position, wvp);
	float3 worldPos = mul(input.Position, world).xyz;
	
	// Light direction
	output.LPos0  = light_pos0 - worldPos;
	
	// Camera direction
	output.CamDir = cam_pos - worldPos;

	// Texture
	output.Texture0 = input.Texture0;
	
	// Normal
	output.Normal = mul(float4(input.Normal, 0), world).xyz;

	return output;
}


// Fragment Uniforms.
//
uniform float4 ambient;
uniform float4 diffuse;
uniform float4 specular;
uniform float spec_comp;

float4 GetColor(float3 LPos, float3 camDir, float3 N, float3 color, float4 attn)
{
	float dist = max(0.0, length(LPos)-attn.w);

	LPos = normalize(LPos);
	float3 H = normalize(LPos + camDir);

	// Attenuation
	float den = attn.x + (attn.y*2.0*dist)/attn.w + (attn.z*dist*dist)/(attn.w*attn.w);
	float attnFinal = clamp(1.0 / (den*den), 0.0, 1.0);

	// Ambient
	float4 ambientFinal = ambient;

	// Diffuse
	float intensity = max(0.0, dot(N, LPos));
	float4 diffuseFinal = diffuse * intensity;

	// Specular
	float4 specularFinal = max(specular * pow(saturate(dot(N, H)), spec_comp), 0.0);

	return saturate(float4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color, 1));
}

float4 MyPS(VSOutput input):SV_Target
{	
	// Camera Direction
	input.CamDir = normalize(input.CamDir);

	// Normal
	float3 N = normalize(input.Normal);

	// Attenuation
	float4 color0 = GetColor(input.LPos0, input.CamDir, N, light_color0, light_attn0);
	
	// Final fragment color
	return tex0.Sample(defss, input.Texture0) * color0;
}

technique11 MyTechnique
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, MyVS()));
		SetGeometryShader(0);
		SetPixelShader(CompileShader(ps_4_0, MyPS()));
    }
}