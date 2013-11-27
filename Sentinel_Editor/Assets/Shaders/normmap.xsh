#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
	matrix wvp;
	matrix world;
	
	float3 light_pos0;
	float3 light_color0;
	float4 light_attn0;
	
	float3 cam_pos;

	float4 ambient;
	float4 diffuse;
	float4 specular;
	float  spec_comp;
}


// Textures.
//
Texture2D    tex0		:register(t0);
SamplerState sampler0	:register(s0);

Texture2D    tex1		:register(t1);
SamplerState sampler1	:register(s1);


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

VSOutput VS_Main( VSInput input )
{
	VSOutput output;

	output.Position = mul(wvp,   input.Position);
	float3 worldPos = mul(world, input.Position).xyz;

	// Transform light direction into tangent space.
	//
	float3 normal   = mul(world, float4(input.Normal.xyz, 0)).xyz;
	float3 tangent  = mul(world, float4(input.Tangent.xyz, 0)).xyz;
	float3 binormal = cross(normal, tangent.xyz) * input.Tangent.w;

	float3x3 matTBN = float3x3(tangent.x, binormal.x, normal.x, 
							   tangent.y, binormal.y, normal.y, 
							   tangent.z, binormal.z, normal.z);

	// Camera direction
	output.CamDir = mul(matTBN, cam_pos - worldPos);

	// Light Position
	float3 LPos   = light_pos0 - worldPos;
	output.LPos0  = mul(matTBN, LPos);
	output.LDist0 = LPos;

	// Texture color
	output.Texture0 = input.Texture0;

	return output;
}


// Pixel Shader.
//
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

float4 PS_Main(VSOutput input):SV_Target
{
	// Camera Direction
	input.CamDir = normalize(input.CamDir);

	// Normal
	float3 N = normalize(tex1.Sample(sampler1, input.Texture0).xyz * 2.0 - 1.0);
	
	// Attenuation
	float4 color0 = GetColor(input.LPos0, input.LDist0, input.CamDir, N, light_color0, light_attn0);
	
	// Final fragment color
	return tex0.Sample(sampler0, input.Texture0) * color0;
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

varying vec2 vTexture0;
varying vec3 vNormal;
varying vec3 vCamDir;
varying vec3 vLPos0;
varying vec3 vLDist0;

#ifdef VERTEX_SHADER

uniform mat4 wvp;
uniform mat4 world;

uniform vec3 cam_pos;
uniform vec3 light_pos0;

attribute vec4 aPosition;
attribute vec2 aTexture0;
attribute vec3 aNormal;
attribute vec4 aTangent;

void main()
{
	gl_Position   = mul(wvp, aPosition);
	vec3 worldPos = mul(world, aPosition).xyz;

	// Transform light direction into tangent space.
	//
	vec3 normal   = mul(world, vec4(aNormal.xyz, 0)).xyz;
	vec3 tangent  = mul(world, vec4(aTangent.xyz, 0)).xyz;
	vec3 binormal = cross(normal, tangent.xyz) * aTangent.w;

	mat3 matTBN = mat3(tangent.x, binormal.x, normal.x, 
					   tangent.y, binormal.y, normal.y, 
					   tangent.z, binormal.z, normal.z);

	// Camera direction
	vCamDir = mul(matTBN, cam_pos - worldPos);

	// Light Position
	vec3 LPos = light_pos0 - worldPos;
	vLPos0    = mul(matTBN, LPos);
	vLDist0   = LPos;

	// Texture color
	vTexture0 = aTexture0;
}

#endif
#ifdef FRAGMENT_SHADER

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float spec_comp;

uniform vec3 light_color0;
uniform vec4 light_attn0;

vec4 GetColor( vec3 LPos, vec3 LDist, vec3 camDir, vec3 N, vec3 color, vec4 attn )
{
	float dist = max(0.0, length(LDist)-attn.w);

	LPos = normalize(LPos);
	vec3 H = normalize(LPos + camDir);

	// Attenuation
	float den = attn.x + (attn.y*2.0*dist)/attn.w + (attn.z*dist*dist)/(attn.w*attn.w);
	float attnFinal = clamp(1.0 / (den*den), 0.0, 1.0);

	// Ambient
	vec4 ambientFinal = ambient;

	// Diffuse
	float intensity = clamp(dot(N, LPos), 0.0, 1.0);
	vec4 diffuseFinal = diffuse * intensity;

	// Specular
	vec4 specularFinal = max(specular * pow(clamp(dot(N, H), 0.0, 1.0), spec_comp), 0.0);

	return clamp(vec4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color, 1.0), 0.0, 1.0);
}

void main()
{
	// Camera Direction
	vec3 camDir = normalize(vCamDir);

	// Normal
	vec3 N = normalize(texture2D(tex1, vTexture0).xyz * 2.0 - 1.0);

	// Attenuation
	vec4 color0 = GetColor(vLPos0, vLDist0, camDir, N, light_color0, light_attn0);

	// Final fragment color
	gl_FragColor = texture2D(tex0, vTexture0) * color0;
}

#endif
#endif
