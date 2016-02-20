#ifdef VERSION_DX

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


// Vertex Shader.
//
struct VSInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float3 CameraPos : CAMERA_POS;
	float3 LightPos : LIGHT_POS;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;

	// Position
	output.Position = mul(_WorldViewProj, input.Position);
	float3 worldPos = mul(_World, input.Position).xyz;

	// Light direction
	output.LightPos = _LightPos - worldPos;

	// Camera direction
	output.CameraPos = _CameraPos - worldPos;

	// Normal
	output.Normal = mul(_World, float4(input.Normal, 0)).xyz;

	return output;
}


// Pixel Shader.
//
float4 GetColor(float3 LPos, float3 camPos, float3 N, float3 color, float4 attn)
{
	if (dot(-LPos, N) < 0)
	{
		float dist = max(0.0, length(LPos) - attn.w);

		LPos = normalize(LPos);
		float3 H = normalize(LPos + camPos);

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

		return saturate(float4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color,
			_Ambient.a + _Diffuse.a + _Specular.a));
	}
	else return _Ambient;
}

float4 PS_Main(VSOutput input) : SV_Target
{
	// Camera Direction
	float3 camPos = normalize(input.CameraPos);

	// Normal
	float3 N = normalize(input.Normal);

	// Attenuation
	float4 color0 = GetColor(input.LightPos, camPos, N, _LightColor, _LightAttn);

	// Final fragment color
	return color0;
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
in vec3 Normal;

out vec3 vNormal;
out vec3 vWorldPos;
out vec3 vCameraDir;
out vec3 vLightDir;

void main()
{
	gl_Position = _WorldViewProj * vec4(Position, 1);
	vWorldPos = (_World * vec4(Position, 1)).xyz;

	// Light direction
	vLightDir = _LightPos - vWorldPos.xyz;

	// Camera direction
	vCameraDir = _CameraPos - vWorldPos.xyz;

	// Normal
	vNormal = (_World * vec4(Normal, 0)).xyz;
}

#endif
#ifdef FRAGMENT_SHADER

const float BLEND = 2.0;
const float BLEND_INC = 1.0;
const float BLEND_DENOM = (BLEND / BLEND_INC) * 2.0 + 1.0;
const float BLEND_FACTOR = 1.0 / (BLEND_DENOM * BLEND_DENOM);

const float INV_SHADOW_SIZE = 1.0 / 512.0;
const float MIN_VARIANCE = 0.0001;
const float SHADOW_STEP = 0.5;

uniform samplerCube _TextureCube;

uniform vec3 _LightColor;
uniform vec4 _LightAttn;

uniform vec4 _Ambient;
uniform vec4 _Diffuse;
uniform vec4 _Specular;
uniform float _SpecComp;

in vec3 vNormal;
in vec3 vWorldPos;
in vec3 vCameraDir;
in vec3 vLightDir;

out vec4 vFragColor;

float CalculateShadow(float lightDepth, vec3 lightDir)
{
	vec2 moments = texture(_TextureCube, lightDir).rg;

	float p = (lightDepth <= moments.x) ? 1 : 0;

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, MIN_VARIANCE);

	float d = lightDepth - moments.x;
	float p_max = variance / (variance + d*d);

	return max(p, (p_max - SHADOW_STEP) / (1.0 - SHADOW_STEP));
}

vec3 GetColor(vec3 normal, vec3 lightDir, vec3 cameraDir, vec3 lightColor, vec4 lightAttn)
{
	vec3 result;
	vec3 lightNormal = normalize(lightDir);
	float intensity = clamp(dot(normal, lightNormal), 0.0, 1.0);

	if (intensity > 0)
	{
		float d = length(lightDir);
		float r = lightAttn.w;

		// Attenuation
		// Extend light beyond radius using standard attenuation.
		//d = max(0.0, d - r);
		//float attenuation = lightAttn.x + (lightAttn.y*d)/r + (lightAttn.z*d*d)/(r*r);

		// Set to end light at radius.
		float attenuation = clamp(1.0 - d*d / (r*r), 0.0, 1.0);
		attenuation *= attenuation;

		// Specular
		vec3 specularFinal = max(_Specular.rgb * pow(clamp(dot(normal, normalize(lightNormal + cameraDir)), 0.0, 1.0), _SpecComp), 0.0);

		// Shadow with PCF blend
		float lightDepth = d / r;
		float shadow = CalculateShadow(lightDepth, -lightNormal);

		vec3 right = lightNormal.yzx * INV_SHADOW_SIZE;
		vec3 up = lightNormal.zxy * INV_SHADOW_SIZE;

		for (float offsetU = -BLEND; offsetU <= BLEND; offsetU += BLEND_INC)
		{
			for (float offsetV = -BLEND; offsetV <= BLEND; offsetV += BLEND_INC)
			{
				shadow += CalculateShadow(lightDepth, -lightNormal + right * offsetU + up * offsetV);
			}
		}

		// Result
		result = clamp((_Diffuse.rgb * intensity + specularFinal.rgb) * attenuation * lightColor, 0.0, 1.0) * clamp(shadow * BLEND_FACTOR, 0.0, 1.0);
	}
	
	return result;
}

void main()
{
	// Normal
	vec3 normal = normalize(vNormal);

	// Camera Direction
	vec3 cameraDir = normalize(vCameraDir);

	// Lighting
	vec3 color0 = GetColor(normal, vLightDir, cameraDir, _LightColor, _LightAttn);

	// Result
	vFragColor = _Ambient + vec4(color0, 0);
}

#endif
#endif