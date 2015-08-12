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
Texture2D _Texture0 : register(t0);
SamplerState _Sampler0 : register(s0);


// Vertex Shader.
//
struct VSInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord0 : TEXCOORD0;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float2 TexCoord0 : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 CameraDir : CAMERA_DIR;
	float3 LightPos : LIGHT_POS;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;

	// Position
	output.Position = mul(_WVP, input.Position);
	float3 worldPos = mul(_World, input.Position).xyz;

	// Light direction
	output.LightPos = _LightPos - worldPos;

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
	float dist = max(0.0, length(LPos) - attn.w);

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
#ifdef VERTEX_SHADER

uniform mat4 _WVP;
uniform mat4 _World;

uniform vec3 _LightPos;
uniform vec3 _CameraPos;

in vec3 Position;
in vec2 TexCoord0;
in vec3 Normal;

out vec2 vTexCoord0;
out vec3 vNormal;
out vec3 vWorldPos;
out vec3 vCamDir;
out vec3 vLPos0;

void main()
{
	gl_Position = _WVP * vec4(Position, 1);
	vWorldPos = (_World * vec4(Position, 1)).xyz;

	// Light direction
	vLPos0 = _LightPos - vWorldPos;

	// Camera direction
	vCamDir = _CameraPos - vWorldPos;

	// Texture
	vTexCoord0 = TexCoord0;

	// Normal
	vNormal = (_World * vec4(Normal, 0)).xyz;
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

in vec2 vTexCoord0;
in vec3 vNormal;
in vec3 vWorldPos;
in vec3 vCamDir;
in vec3 vLPos0;

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

		// Specular
		vec3 specularFinal = max(_Specular.rgb * pow(clamp(dot(normal, normalize(lightDir + cameraDir)), 0.0, 1.0), _SpecComp), 0.0);

		return clamp((_Diffuse.rgb * intensity + specularFinal.rgb) * attenuation * color, 0.0, 1.0);
	}
	else return vec3(0, 0, 0);
}

void main()
{
	// Camera Direction
	vec3 camDir = normalize(vCamDir);

	// Normal
	vec3 N = normalize(vNormal);

	// Attenuation
	vec3 color0 = GetColor(vLPos0, camDir, N, _LightColor, _LightAttn);

	// Final fragment color
	vFragColor = (_Ambient + vec4(color0, 0)) * texture2D(_Texture0, vTexCoord0);
}

#endif
#endif
