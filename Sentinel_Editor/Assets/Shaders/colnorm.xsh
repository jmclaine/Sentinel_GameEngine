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


// Vertex Shader.
//
struct VSInput
{
	float4 Position	:POSITION;
	float3 Normal	:NORMAL;
};

struct VSOutput
{
	float4 Position	:SV_POSITION;
	float3 Normal	:NORMAL0;
	float3 CamDir	:NORMAL1;
	float3 LPos0	:NORMAL2;
};

VSOutput VS_Main( VSInput input )
{
	VSOutput output;

	// Position
	output.Position = mul(wvp,   input.Position);
	float3 worldPos = mul(world, input.Position).xyz;
	
	// Light direction
	output.LPos0  = light_pos0 - worldPos;
	
	// Camera direction
	output.CamDir = cam_pos - worldPos;
	
	// Normal
	output.Normal = mul(world, float4(input.Normal, 0)).xyz;

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
	float4 ambientFinal = ambient;

	// Diffuse
	float intensity = max(0.0, dot(N, LPos));
	float4 diffuseFinal = diffuse * intensity;

	// Specular
	float4 specularFinal = max(specular * pow(saturate(dot(N, H)), spec_comp), 0.0);

	return saturate(float4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color, 
					ambientFinal.a + diffuseFinal.a + specularFinal.a));
}

float4 PS_Main(VSOutput input):SV_Target
{	
	// Camera Direction
	input.CamDir = normalize(input.CamDir);

	// Normal
	float3 N = normalize(input.Normal);

	// Attenuation
	float4 color0 = GetColor(input.LPos0, input.CamDir, N, light_color0, light_attn0);
	
	// Final fragment color
	return color0;
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

varying vec3 vNormal;
varying vec3 vCamDir;
varying vec3 vLPos0;

#ifdef VERTEX_SHADER

uniform mat4 wvp;
uniform mat4 world;

uniform vec3 light_pos0;
uniform vec3 cam_pos;

attribute vec4 aPosition;
attribute vec3 aNormal;

void main()
{
	gl_Position   = mul(wvp, aPosition);
	vec3 worldPos = mul(world, aPosition);

	// Light direction
	vLPos0  = light_pos0 - worldPos;
	
	// Camera direction
	vCamDir = cam_pos - worldPos;

	// Normal
	vNormal = mul(world, vec4(aNormal, 0));
}

#endif
#ifdef FRAGMENT_SHADER

uniform vec3 light_color0;
uniform vec4 light_attn0;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float spec_comp;

vec4 GetColor( vec3 LPos, vec3 camDir, vec3 N, vec3 color, vec4 attn )
{
	float dist = max(0.0, length(LPos)-attn.w);

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

	return clamp(vec4(ambientFinal.rgb + (diffuseFinal.rgb + specularFinal.rgb) * attnFinal * color, 
				 ambient.a + diffuse.a + specular.a), 0.0, 1.0);
}

void main()
{
	// Camera Direction
	vec3 camDir = normalize(vCamDir);

	// Normal
	vec3 N = normalize(vNormal);

	// Attenuation
	vec4 color0 = GetColor(vLPos0, camDir, N, light_color0, light_attn0);

	// Final fragment color
	gl_FragColor = color0;
}

#endif
#endif