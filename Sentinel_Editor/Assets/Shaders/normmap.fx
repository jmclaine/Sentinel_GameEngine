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
Texture2D tex1;
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
	float4 Tangent	:TANGENT;
};

struct VSOutput
{
	float4 Position	:SV_POSITION;
	float2 Texture0	:TEXCOORD0;
	float3 CamDir	:NORMAL1;
	float3 LPos0	:NORMAL2;
	float3 LDist0	:NORMAL3;
};

VSOutput MyVS( VSInput input )
{
	VSOutput output;

	output.Position = mul(input.Position, wvp);
	float3 worldPos = mul(input.Position, world).xyz;

	// Transform light direction into tangent space.
	//
	float3 normal   = mul(float4(input.Normal.xyz, 0), world).xyz;
	float3 tangent  = mul(float4(input.Tangent.xyz, 0), world).xyz;
	float3 binormal = cross(normal, tangent.xyz) * input.Tangent.w;

	float3x3 matTBN = float3x3(tangent.x, binormal.x, normal.x, 
							   tangent.y, binormal.y, normal.y, 
							   tangent.z, binormal.z, normal.z);

	// Camera direction
	output.CamDir = mul(cam_pos - worldPos, matTBN);

	// Light Position
	float3 LPos   = light_pos0 - worldPos;
	output.LPos0  = mul(LPos, matTBN);
	output.LDist0 = LPos;

	// Texture color
	output.Texture0 = input.Texture0;

	return output;
}


// Fragment Shader.
//
uniform float4 ambient;
uniform float4 diffuse;
uniform float4 specular;
uniform float spec_comp;

float4 GetColor( float3 LPos, float3 LDist, float3 camDir, float3 N, float3 color, float4 attn )
{
	float dist = max(0.0, length(LDist)-attn.w);
	
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
	float3 N = normalize(tex1.Sample(defss, input.Texture0).xyz * 2.0 - 1.0);
	
	// Attenuation
	float4 color0 = GetColor(input.LPos0, input.LDist0, input.CamDir, N, light_color0, light_attn0);
	
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