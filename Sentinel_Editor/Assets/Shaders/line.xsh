#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
    matrix _WVP;

	float4 _Ambient;
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

	output.Position = mul(_WVP, input.Position);
	
	return output;
}


// Pixel Shader.
//
float4 PS_Main(VSOutput input):SV_Target
{	
	return saturate(_Ambient);
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

#ifdef VERTEX_SHADER

uniform mat4 _WVP;

attribute vec4 Position;

void main()
{
	gl_Position = _WVP * Position;
}

#endif
#ifdef FRAGMENT_SHADER

uniform vec4 _Ambient;

void main()
{
	gl_FragColor = clamp(_Ambient, 0.0, 1.0);
}

#endif
#endif
