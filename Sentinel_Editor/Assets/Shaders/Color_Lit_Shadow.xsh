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
varying vec4 vWorldPos;
varying vec3 vCameraDir;
varying vec3 vLightDir;

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
	vLightDir = _LightPos - vWorldPos.xyz;
	
	// Camera direction
	vCameraDir = _CameraPos - vWorldPos.xyz;

	// Normal
	vNormal = mul(_World, vec4(Normal, 0)).xyz;
}

#endif
#ifdef FRAGMENT_SHADER

const float blend = 2.0;
const float blendInc = 1.0;

const float blendDenom = (blend / blendInc) * 2.0 + 1.0;
const float blendFactor = 1.0 / (blendDenom * blendDenom);

const float shadowRadius = 25.0;

uniform samplerCube _TextureCube;

uniform vec3 _LightColor;
uniform vec4 _LightAttn;

uniform vec4 _Ambient;
uniform vec4 _Diffuse;
uniform vec4 _Specular;
uniform float _SpecComp;

vec3 GetColor( vec3 lightDir, vec3 cameraDir, vec3 normal, vec3 color, vec4 attn, float sum )
{
	float intensity = clamp( dot( normal, lightDir ), 0.0, 1.0 );

	if( intensity > 0 )
	{
		float d = distance( lightDir, vWorldPos );
		float r = attn.w;

		// Attenuation
		// Extend light beyond radius using standard attenuation.
		//d = max( 0.0, d - attn.w );
		//float attenuation = attn.x + (attn.y*d)/r + (attn.z*d*d)/(r*r);

		// Set to end light at radius.
		float attenuation = clamp( 1.0 - d*d/(r*r), 0.0, 1.0 );
		attenuation *= attenuation;

		// Specular
		vec3 specularFinal = max(_Specular.rgb * pow( clamp( dot( normal, normalize( lightDir + cameraDir )), 0.0, 1.0 ), _SpecComp ), 0.0);

		return clamp((_Diffuse.rgb * intensity + specularFinal.rgb) * attenuation * color, 0.0, 1.0);
	}
	else
		return vec3(0, 0, 0);
}

float CalculateShadow( float offsetU, float offsetV, float lightDepth, vec3 tc )
{
	float depth = texture( _TextureCube, tc ).r;
	
	float dx = dFdx(depth);
	float dy = dFdy(depth);

	float p = lightDepth <= depth;

	float variance = 0.25*(dx*dx + dy*dy);
	variance = max( variance, 0.0 );

	float dist = lightDepth - depth;

	float p_max = variance / (variance + dist*dist);

	return max(p, p_max);
	//*/
	/*
	if( lightDepth < depth )
		return 1;

	return 0;
	//*/
	/*
	vec2 moments = texture( _TextureCube, tc ).rg;
	
	float p = lightDepth <= moments.x;

	float variance = moments.y;// - (moments.x * moments.x);
	variance = max( variance, 0.0 );

	float dist = lightDepth - moments.x;

	float p_max = variance / (variance + dist*dist);

	return max(p, p_max);
	//*/
}

void main()
{
	float lightDepth = length(vLightDir) / shadowRadius;
	vec3 lightDir = normalize(vLightDir);

	// Camera Direction
	vec3 cameraDir = normalize(vCameraDir);

	// Normal
	vec3 normal = normalize(vNormal);

	// Shadows
	vec3 right = vec3( vLightDir.y, vLightDir.z, vLightDir.x )*0.0009765625;
	vec3 up    = vec3( vLightDir.z, vLightDir.x, vLightDir.y )*0.0009765625;
	
	float shadow = 0.0;
	
	// PCF blend
	for( float offsetU = -blend; offsetU <= blend; offsetU += blendInc )
	{
		for( int offsetV = -blend; offsetV <= blend; offsetV += blendInc )
		{
			shadow += CalculateShadow( offsetU, offsetV, lightDepth, -vLightDir + right*offsetU + up*offsetV );
		}
	}

	shadow *= blendFactor;
	//*/
	//float shadow = CalculateShadow( 0, 0, lightDepth, -vLightDir );
	
	vec3 color0 = GetColor( lightDir, cameraDir, normal, _LightColor, _LightAttn, shadow );
	gl_FragColor = _Ambient + vec4(color0 * shadow, 1);
	//gl_FragColor = vec4(shadow, 0, 0, 1);
}

#endif
#endif