#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
	matrix _WVP;
	matrix _World;
	
	float3 _LightPos;
	float3 _LightColor;
	float4 _LightAttn;
	
	float3 _CameraPos;

	float4 _Ambient;
	float4 _Diffuse;
	float4 _Specular;
	float  _SpecComp;
}


// Textures.
//
Texture2D    _Texture0	:register(t0);
SamplerState _Sampler0	:register(s0);

Texture2D    _Texture1	:register(t1);
SamplerState _Sampler1	:register(s1);


// Vertex Shader.
//
struct VSInput
{
	float4 Position		:POSITION;
	float2 TexCoord0	:TEXCOORD0;
	float3 Normal		:NORMAL;
	float4 Tangent		:TANGENT;
};

struct VSOutput
{
	float4 Position		:SV_POSITION;
	float2 TexCoord0	:TEXCOORD0;
	float3 CamDir		:NORMAL1;
	float3 LPos0		:NORMAL2;
	float3 LDist0		:NORMAL3;
};

VSOutput VS_Main( VSInput input )
{
	VSOutput output;

	output.Position = mul(_WVP,   input.Position);
	float3 worldPos = mul(_World, input.Position).xyz;

	// Transform light direction into tangent space.
	//
	float3 normal   = mul(_World, float4(input.Normal.xyz, 0)).xyz;
	float3 tangent  = mul(_World, float4(input.Tangent.xyz, 0)).xyz;
	float3 binormal = cross(normal, tangent.xyz) * input.Tangent.w;

	float3x3 matTBN = float3x3(tangent.x, binormal.x, normal.x, 
							   tangent.y, binormal.y, normal.y, 
							   tangent.z, binormal.z, normal.z);

	// Camera direction
	output.CamDir = mul(matTBN, _CameraPos - worldPos);

	// Light Position
	float3 LPos   = _LightPos - worldPos;
	output.LPos0  = mul(matTBN, LPos);
	output.LDist0 = LPos;

	// Texture color
	output.TexCoord0 = input.TexCoord0;

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
	float4 ambientFinal = _Ambient;

	// Diffuse
	float intensity = max(0.0, dot(N, LPos));
	float4 diffuseFinal = _Diffuse * intensity;

	// Specular
	float4 specularFinal = max(_Specular * pow(saturate(dot(N, H)), _SpecComp), 0.0);

	return saturate(float4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color, 1));
}

float4 PS_Main( VSOutput input ):SV_Target
{
	// Camera Direction
	input.CamDir = normalize(input.CamDir);

	// Normal
	float3 N = normalize(_Texture1.Sample(_Sampler1, input.TexCoord0).xyz * 2.0 - 1.0);
	
	// Attenuation
	float4 color0 = GetColor(input.LPos0, input.LDist0, input.CamDir, N, _LightColor, _LightAttn);
	
	// Final fragment color
	return _Texture0.Sample(_Sampler0, input.TexCoord0) * color0;
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

varying vec2 vTexCoord0;
varying vec3 vNormal;
varying vec3 vCamDir;
varying vec3 vLPos0;
varying vec3 vLDist0;

#ifdef VERTEX_SHADER

uniform mat4 _WVP;
uniform mat4 _World;

uniform vec3 _CameraPos;
uniform vec3 _LightPos;

attribute vec4 Position;
attribute vec2 TexCoord0;
attribute vec3 Normal;
attribute vec4 Tangent;

void main()
{
	gl_Position   = mul(_WVP, Position);
	vec3 worldPos = mul(_World, Position).xyz;

	// Transform light direction into tangent space.
	//
	vec3 normal   = mul(_World, vec4(Normal.xyz, 0)).xyz;
	vec3 tangent  = mul(_World, vec4(Tangent.xyz, 0)).xyz;
	vec3 binormal = cross(normal, tangent.xyz) * Tangent.w;

	mat3 matTBN = mat3(tangent.x, binormal.x, normal.x, 
					   tangent.y, binormal.y, normal.y, 
					   tangent.z, binormal.z, normal.z);

	// Camera direction
	vCamDir = mul(matTBN, _CameraPos - worldPos);

	// Light Position
	vec3 LPos = _LightPos - worldPos;
	vLPos0    = mul(matTBN, LPos);
	vLDist0   = LPos;

	// Texture color
	vTexCoord0 = TexCoord0;
}

#endif
#ifdef FRAGMENT_SHADER

uniform sampler2D _Texture0;
uniform sampler2D _Texture1;

uniform vec4 _Ambient;
uniform vec4 _Diffuse;
uniform vec4 _Specular;
uniform float _SpecComp;

uniform vec3 _LightColor;
uniform vec4 _LightAttn;

vec4 GetColor( vec3 LPos, vec3 LDist, vec3 camDir, vec3 N, vec3 color, vec4 attn )
{
	float dist = max(0.0, length(LDist)-attn.w);

	LPos = normalize(LPos);
	vec3 H = normalize(LPos + camDir);

	// Attenuation
	float den = attn.x + (attn.y*2.0*dist)/attn.w + (attn.z*dist*dist)/(attn.w*attn.w);
	float attnFinal = clamp(1.0 / (den*den), 0.0, 1.0);

	// Ambient
	vec4 ambientFinal = _Ambient;

	// Diffuse
	float intensity = clamp(dot(N, LPos), 0.0, 1.0);
	vec4 diffuseFinal = _Diffuse * intensity;

	// Specular
	vec4 specularFinal;
	if(dot(-LPos, N) < 0)
		specularFinal = max(_Specular * pow(clamp(dot(N, H), 0.0, 1.0), _SpecComp), 0.0);

	return clamp(vec4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color, 1.0), 0.0, 1.0);
}

void main()
{
	// Camera Direction
	vec3 camDir = normalize(vCamDir);

	// Normal
	vec3 N = normalize(texture2D(_Texture1, vTexCoord0).xyz * 2.0 - 1.0);

	// Attenuation
	vec4 color0 = GetColor(vLPos0, vLDist0, camDir, N, _LightColor, _LightAttn);

	// Final fragment color
	gl_FragColor = texture2D(_Texture0, vTexCoord0) * color0;
}

#endif
#endif
