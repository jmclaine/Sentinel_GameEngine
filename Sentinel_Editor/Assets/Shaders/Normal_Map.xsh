#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
	matrix _WorldViewProj;
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
Texture2D _Texture0 : register(t0);
SamplerState _Sampler0 : register(s0);

Texture2D _Texture1 : register(t1);
SamplerState _Sampler1 : register(s1);


// Vertex Shader.
//
struct VSInput
{
	float4 Position : POSITION;
	float2 TexCoord0 : TEXCOORD0;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float2 TexCoord0 : TEXCOORD0;
	float3 CamDir : NORMAL1;
	float3 LPos0 : NORMAL2;
	float3 LDist0 : NORMAL3;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;

	output.Position = mul(_WorldViewProj, input.Position);
	float3 worldPos = mul(_World, input.Position).xyz;

	// Transform light direction into tangent space.
	//
	float3 normal = mul(_World, float4(input.Normal.xyz, 0)).xyz;
	float3 tangent = mul(_World, float4(input.Tangent.xyz, 0)).xyz;
	float3 binormal = cross(normal, tangent.xyz) * input.Tangent.w;

	float3x3 matTBN = float3x3(
		tangent.x, binormal.x, normal.x,
		tangent.y, binormal.y, normal.y,
		tangent.z, binormal.z, normal.z);

	// Camera direction
	output.CamDir = mul(matTBN, _CameraPos - worldPos);

	// Light Position
	float3 LPos = _LightPos - worldPos;
	output.LPos0 = mul(matTBN, LPos);
	output.LDist0 = LPos;

	// Texture color
	output.TexCoord0 = input.TexCoord0;

	return output;
}


// Pixel Shader.
//
float4 GetColor(float3 LPos, float3 LDist, float3 camDir, float3 N, float3 color, float4 attn)
{
	float dist = max(0.0, length(LDist) - attn.w);

	LPos = normalize(LPos);
	float3 H = normalize(LPos + camDir);

	// Attenuation
	float den = attn.x + (attn.y*2.0*dist) / attn.w + (attn.z*dist*dist) / (attn.w*attn.w);
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

float4 PS_Main(VSOutput input) :SV_Target
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
#ifdef VERTEX_SHADER

uniform mat4 _WorldViewProj;
uniform mat4 _World;

uniform vec3 _LightPos;
uniform vec3 _CameraPos;

in vec3 Position;
in vec2 TexCoord0;
in vec3 Normal;
in vec4 Tangent;

out vec2 vTexCoord0;
out vec3 vWorldPos;
out vec3 vCameraDir;
out vec3 vLightDir;
out mat3 vTBN;

void main()
{
	gl_Position = _WorldViewProj * vec4(Position, 1);
	vWorldPos = (_World * vec4(Position, 1)).xyz;

	vec3 normal = normalize((_World * vec4(Normal.xyz, 0)).xyz);
	vec3 tangent = normalize((_World * vec4(Tangent.xyz, 0)).xyz);
	vec3 bitangent = normalize(cross(normal, tangent.xyz) * Tangent.w);

	vTBN = (mat3(tangent, bitangent, normal));

	vLightDir = (_LightPos - vWorldPos);
	vCameraDir = (_CameraPos - vWorldPos);

	vTexCoord0 = vec2(TexCoord0.x, TexCoord0.y);
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

//uniform vec3 _LightPos;
//uniform vec3 _CameraPos;

in vec2 vTexCoord0;
in vec3 vWorldPos;
in vec3 vCameraDir;
in vec3 vLightDir;
in mat3 vTBN;

out vec4 vFragColor;

vec3 GetColor(vec3 lightDir, vec3 cameraDir, vec3 normal, vec3 color, vec4 attn)
{
	float intensity = clamp(dot(normal, lightDir), 0.0, 1.0);

	if (intensity > 0)
	{
		float d = distance(lightDir, vWorldPos);
		float r = attn.w;

		// Attenuation
		// Extend light beyond radius using standard attenuation.
		//d = max( 0.0, d - r );
		//float attenuation = attn.x + (attn.y*d)/r + (attn.z*d*d)/(r*r);

		// Set to end light at radius.
		float attenuation = clamp(1.0 - d*d / (r*r), 0.0, 1.0);
		attenuation *= attenuation;

		vec3 specular = max(_Specular.rgb * pow(clamp(dot(normal, normalize(lightDir + cameraDir)), 0.0, 1.0), _SpecComp), 0.0);

		return clamp((_Diffuse.rgb * intensity + specular.rgb) * attenuation * color, 0.0, 1.0);
	}
	else return vec3(0, 0, 0);
}

void main()
{
	vec3 lightDir = normalize(vLightDir);
	vec3 cameraDir = normalize(vCameraDir);

	vec3 normal = vTBN * normalize((texture2D(_Texture1, vTexCoord0.xy).rgb * 2.0 - 1.0));

	vec3 color0 = GetColor(lightDir, cameraDir, normal, _LightColor, _LightAttn);

	vFragColor = (_Ambient + vec4(color0, 0)) * texture2D(_Texture0, vTexCoord0);
}
#endif
#endif
