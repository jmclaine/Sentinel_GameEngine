#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
    matrix wvp;

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
};

struct VSOutput
{
	float4 Position	:SV_POSITION;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;

	output.Position = mul(wvp, input.Position);
	
	return output;
}


// Pixel Shader.
//
float4 PS_Main(VSOutput input):SV_Target
{	
	return saturate(ambient);
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

#ifdef VERTEX_SHADER

uniform mat4 wvp;

attribute vec4 aPosition;

void main()
{
	gl_Position = wvp * aPosition;
}

#endif
#ifdef FRAGMENT_SHADER

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float spec_comp;

void main()
{
	gl_FragColor = clamp(ambient, 0.0, 1.0);
}

#endif
#endif
