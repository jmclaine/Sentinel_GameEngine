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


// Vertex Shader.
//
struct VSInput
{
	float4 Position		:POSITION;
	float3 Normal		:NORMAL;
};

struct VSOutput
{
	float4 Position		:SV_POSITION;
	float3 Normal		:NORMAL;
	float3 CameraPos	:CAMERA_POS;
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
	output.CameraPos = _CameraPos - worldPos;
	
	// Normal
	output.Normal = mul(_World, float4(input.Normal, 0)).xyz;

	return output;
}


// Pixel Shader.
//
float4 GetColor(float3 LPos, float3 camPos, float3 N, float3 color, float4 attn)
{
	if(dot(-LPos, N) < 0)
	{
		float dist = max(0.0, length(LPos)-attn.w);

		LPos = normalize(LPos);
		float3 H = normalize(LPos + camPos);

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

		return saturate(float4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color, 
							   _Ambient.a + _Diffuse.a + _Specular.a));
	}
	else
		return _Ambient;
}

float4 PS_Main( VSOutput input ) : SV_Target
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

varying vec3 vNormal;
varying vec3 vCameraPos;
varying vec3 vLightPos;
varying vec4 vWorldPos;

#ifdef VERTEX_SHADER

uniform mat4 _WVP;
uniform mat4 _World;

uniform vec3 _LightPos;
uniform vec3 _CameraPos;

attribute vec3 Position;
attribute vec3 Normal;

void main()
{
	gl_Position = mul(_WVP, vec4(Position, 1));
	vWorldPos   = mul(_World, vec4(Position, 1));

	// Light direction
	vLightPos = _LightPos - vWorldPos.xyz;
	
	// Camera direction
	vCameraPos = _CameraPos - vWorldPos.xyz;

	// Normal
	vNormal = mul(_World, vec4(Normal, 0)).xyz;
}

#endif
#ifdef FRAGMENT_SHADER

/*
// 3x3 Gauss
const float ratio0 = 0.0625;
const float ratio1 = 0.125;
const float ratio2 = 0.0625;

const float ratio3 = 0.125;
const float ratio4 = 0.25;
const float ratio5 = 0.125;

const float ratio6 = 0.0625;
const float ratio7 = 0.125;
const float ratio8 = 0.0625;
//*/
const float blend = 2.0;
const float blendInc = 1.0;

const float blendDenom = (blend / blendInc) * 2.0 + 1.0;
const float blendFactor = 1.0 / (blendDenom * blendDenom);

const float shadowRadius = 200.0;
const float shadowMin = 0.2;

uniform samplerCube _TextureCube;

uniform vec3 _LightColor;
uniform vec4 _LightAttn;

uniform vec4 _Ambient;
uniform vec4 _Diffuse;
uniform vec4 _Specular;
uniform float _SpecComp;

vec4 GetColor( vec3 LPos, vec3 camDir, vec3 N, vec3 color, vec4 attn, float sum )
{
	float intensity = clamp(dot(N, LPos), 0.0, 1.0);

	if(intensity > 0)
	{
		float d = max(0.0, distance(LPos, vWorldPos) - attn.w);
		
		// Attenuation
		//float attenuation = clamp(1.0 - (attn.y*d/attn.w + (attn.z*d*d)/(attn.w*attn.w)), 0.0, 1.0);
		float attenuation = clamp(1.0 - d*d/(attn.w*attn.w), 0.0, 1.0);
		attenuation *= attenuation;

		// Diffuse
		vec4 diffuseFinal = _Diffuse * intensity;

		// Specular
		vec4 specularFinal;
		if( sum > shadowMin )
		{
			specularFinal = max(_Specular * pow(clamp(dot(N, normalize(LPos + camDir)), 0.0, 1.0), _SpecComp), 0.0);
		}

		return clamp(vec4((diffuseFinal.rgb + specularFinal.rgb) * attenuation * color, 
						   _Diffuse.a + _Specular.a), 0.0, 1.0);
	}
	else
		return vec4(0, 0, 0, 0);
}

float CalculateShadow( float offsetU, float offsetV, float lightDepth, vec3 lightDir, vec3 right, vec3 up )
{
	float depth = texture(_TextureCube, -normalize(lightDir+right*offsetU+up*offsetV)).r;
	
	float p = lightDepth <= depth;

	//float dx = dFdx(depth);
	//float dy = dFdy(depth);
	
	float variance = 0.0000001;//max(0.25*(dx*dx + dy*dy), 0.0000001);
	float d = lightDepth - depth;
	float p_max = variance / (variance + d*d);

	return max(p, p_max);
}

void main()
{
	vec4 color0;

	float lightDepth = length(vLightPos) / shadowRadius;
	vec3 lightDir = normalize(vLightPos);

	// Camera Direction
	vec3 camPos = normalize(vCameraPos);

	// Normal
	vec3 N = normalize(vNormal);

	// Shadows
	vec3 right = normalize(cross(lightDir, vec3(lightDir.y, lightDir.z, lightDir.x)))*0.0009765625;
	vec3 up    = normalize(cross(lightDir, right))*0.0009765625;
	
	float shadow = 0.0;
	
	// PCF blend
	for(float offsetU=-blend; offsetU<=blend; offsetU+=blendInc)
	{
		for(int offsetV=-blend; offsetV<=blend; offsetV+=blendInc)
		{
			shadow += CalculateShadow(offsetU, offsetV, lightDepth, lightDir, right, up);
		}
	}

	shadow *= blendFactor;
	//*/
	/*
	// 3x3 Gauss
	shadow += CalculateShadow(-1, -1, lightDepth, lightDir, right, up) * ratio0;
	shadow += CalculateShadow(0, -1, lightDepth, lightDir, right, up) * ratio1;
	shadow +=CalculateShadow(1, -1, lightDepth, lightDir, right, up) * ratio2;

	shadow += CalculateShadow(-1, 0, lightDepth, lightDir, right, up) * ratio3;
	shadow += CalculateShadow(0, 0, lightDepth, lightDir, right, up) * ratio4;
	shadow += CalculateShadow(1, 0, lightDepth, lightDir, right, up) * ratio5;

	shadow += CalculateShadow(-1, 1, lightDepth, lightDir, right, up) * ratio6;
	shadow += CalculateShadow(0, 1, lightDepth, lightDir, right, up) * ratio7;
	shadow += CalculateShadow(1, 1, lightDepth, lightDir, right, up) * ratio8;
	//*/
	color0 = vec4(GetColor(lightDir, camPos, N, _LightColor, _LightAttn, shadow).rgb, 1);
	gl_FragColor = _Ambient + vec4(color0.rgb * shadow, 1);
}

#endif
#endif