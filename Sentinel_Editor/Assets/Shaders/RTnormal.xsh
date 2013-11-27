#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
	matrix world;
}


// Textures.
//
Texture2D    tex0		:register(t0);
SamplerState sampler0	:register(s0);


// Vertex Shader.
//
struct VSInput
{
	float4 Position	:POSITION;
	float2 Texture0	:TEXCOORD0;
};

struct VSOutput
{
	float4 Position	:SV_POSITION;
	float2 Texture0	:TEXCOORD0;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;

	output.Position = mul(world, input.Position);
	output.Texture0 = input.Texture0;

	return output;
}


// Pixel Shader.
//
float4 PS_Main(VSOutput input):SV_Target
{
	return float4(tex0.Sample(sampler0, input.Texture0).xyz, 1);
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

#version 330

#ifdef VERTEX_SHADER

uniform mat4 world;

in vec4 aPosition;
in vec2 aTexture0;

out vec2 vTex0;

void main()
{
	gl_Position = world * aPosition;

	vTex0 = vec2(aTexture0.x, 1.0-aTexture0.y);
}

#endif
#ifdef FRAGMENT_SHADER

uniform sampler2D tex0;

in vec2 vTex0;

out vec4 vFragColor;

void main()
{
	vFragColor = vec4(texture2D(tex0, vTex0).xyz, 1);
}

#endif
#endif
