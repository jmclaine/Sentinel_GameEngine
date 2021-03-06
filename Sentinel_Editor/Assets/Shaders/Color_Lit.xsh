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

	output.Position = mul(_WorldViewProj, input.Position);
	float3 worldPos = mul(_World, input.Position).xyz;

	output.LightPos = _LightPos - worldPos;
	output.CameraPos = _CameraPos - worldPos;

	output.Normal = mul(_World, float4(input.Normal, 0)).xyz;

	return output;
}


float4 GetColor(float3 LPos, float3 camPos, float3 N, float3 color, float4 attn)
{
	if (dot(-LPos, N) < 0)
	{
		float dist = max(0.0, length(LPos) - attn.w);

		LPos = normalize(LPos);
		float3 H = normalize(LPos + camPos);

		float den = attn.x + (attn.y*2.0*dist) / attn.w + (attn.z*dist*dist) / (attn.w*attn.w);
		float attnFinal = clamp(1.0 / (den*den), 0.0, 1.0);

		float4 ambientFinal = _Ambient;

		float intensity = max(0.0, dot(N, LPos));
		float4 diffuseFinal = _Diffuse * intensity;

		float4 specularFinal = max(_Specular * pow(saturate(dot(N, H)), _SpecComp), 0.0);

		return saturate(float4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color,
			_Ambient.a + _Diffuse.a + _Specular.a));
	}
	else return _Ambient;
}

float4 PS_Main(VSOutput input) : SV_Target
{
	float3 camPos = normalize(input.CameraPos);

	float3 N = normalize(input.Normal);

	float4 color0 = GetColor(input.LightPos, camPos, N, _LightColor, _LightAttn);

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
out vec3 vCameraPos;
out vec3 vLightPos;

void main()
{
	gl_Position = _WorldViewProj * vec4(Position, 1);
	vWorldPos = (_World * vec4(Position, 1)).xyz;

	// Light direction
	vLightPos = _LightPos - vWorldPos;

	// Camera direction
	vCameraPos = _CameraPos - vWorldPos;

	// Normal
	vNormal = (_World * vec4(Normal, 0)).xyz;
}

#endif
#ifdef FRAGMENT_SHADER

uniform vec3 _LightColor;
uniform vec4 _LightAttn;

uniform vec4 _Ambient;
uniform vec4 _Diffuse;
uniform vec4 _Specular;
uniform float _SpecComp;

in vec3 vNormal;
in vec3 vWorldPos;
in vec3 vCameraPos;
in vec3 vLightPos;

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
	vec3 camPos = normalize(vCameraPos);

	// Normal
	vec3 N = normalize(vNormal);

	// Attenuation
	vec3 color0 = GetColor(vLightPos, camPos, N, _LightColor, _LightAttn);

	// Final fragment color
	vFragColor = _Ambient + vec4(color0, 0);
}

#endif
#endif