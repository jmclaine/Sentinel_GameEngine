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


// Vertex Shader.
//
struct VSInput
{
	float4 Position		:POSITION;
	float2 TexCoord0	:TEXCOORD0;
	float3 Normal		:NORMAL;
};

struct VSOutput
{
	float4 Position		:SV_POSITION;
	float2 TexCoord0	:TEXCOORD0;
	float3 Normal		:NORMAL;
	float3 CameraDir	:CAMERA_DIR;
	float3 LightPos		:LIGHT_POS;
};

VSOutput VS_Main( VSInput input )
{
	VSOutput output;

	// Position
	output.Position = mul(_WVP,   input.Position);
	float3 worldPos = mul(_World, input.Position).xyz;
	
	// Light direction
	output.LightPos  = _LightPos - worldPos;
	
	// Camera direction
	output.CameraDir = _CameraPos - worldPos;

	// Texture
	output.TexCoord0 = input.TexCoord0;
	
	// Normal
	output.Normal = mul(_World, float4(input.Normal, 0)).xyz;

	return output;
}


// Pixel Shader.
//
float4 GetColor(float3 LPos, float3 camDir, float3 N, float3 color, float4 attn)
{
	float dist = max(0.0, length(LPos)-attn.w);

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

float4 PS_Main(VSOutput input):SV_Target
{	
	// Camera Direction
	input.CameraDir = normalize(input.CameraDir);

	// Normal
	float3 N = normalize(input.Normal);

	// Attenuation
	float4 color0 = GetColor(input.LightPos, input.CameraDir, N, _LightColor, _LightAttn);
	
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

#ifdef VERTEX_SHADER

uniform mat4 _WVP;
uniform mat4 _World;

uniform vec3 _LightPos;
uniform vec3 _CameraPos;

attribute vec4 Position;
attribute vec2 TexCoord0;
attribute vec3 Normal;

void main()
{
	gl_Position   = mul(_WVP, Position);
	vec3 worldPos = mul(_World, Position);

	// Light direction
	vLPos0  = _LightPos - worldPos;
	
	// Camera direction
	vCamDir = _CameraPos - worldPos;

	// Texture
	vTexCoord0 = TexCoord0;

	// Normal
	vNormal = mul(_World, vec4(Normal, 0));
}

#endif
#ifdef FRAGMENT_SHADER

uniform sampler2D _Texture0;

uniform vec4 _Ambient;
uniform vec4 _Diffuse;
uniform vec4 _Specular;
uniform float _SpecComp;

uniform vec3 _LightColor;
uniform vec4 _LightAttn;

vec4 GetColor( vec3 LPos, vec3 camDir, vec3 N, vec3 color, vec4 attn )
{
	float dist = max(0.0, length(LPos)-attn.w);

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
	vec3 N = normalize(vNormal);

	// Attenuation
	vec4 color0 = GetColor(vLPos0, camDir, N, _LightColor, _LightAttn);

	// Final fragment color
	gl_FragColor = texture2D(_Texture0, vTexCoord0) * color0;
}

#endif
#endif
