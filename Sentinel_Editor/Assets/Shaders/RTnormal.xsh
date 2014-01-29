#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
	matrix _World;
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
	float2 Texture0		:TEXCOORD0;
};

struct VSOutput
{
	float4 Position		:SV_POSITION;
	float2 Texture0		:TEXCOORD0;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;

	output.Position = mul(_World, input.Position);
	output.Texture0 = input.Texture0;

	return output;
}


// Pixel Shader.
//
float4 PS_Main(VSOutput input):SV_Target
{
	return float4(_Texture0.Sample(_Sampler0, input.Texture0).xyz, 1);
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

#version 330

#ifdef VERTEX_SHADER

uniform mat4 _World;

in vec4 Position;
in vec2 TexCoord0;

out vec2 vTexCoord0;

void main()
{
	gl_Position = _World * Position;

	vTexCoord0 = vec2(TexCoord0.x, 1.0-TexCoord0.y);
}

#endif
#ifdef FRAGMENT_SHADER

uniform sampler2D _Texture0;

in vec2 vTexCoord0;

out vec4 vFragColor;

void main()
{
	vFragColor = vec4(texture2D(_Texture0, vTexCoord0).xyz, 1);
}

#endif
#endif
