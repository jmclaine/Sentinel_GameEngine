#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
    matrix _WorldViewProj;

	float4 _Ambient;
}


// Vertex Shader.
//
struct VSInput
{
	float3 Position	:POSITION;
};

struct VSOutput
{
	float4 Position	:SV_POSITION;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;

	output.Position = mul(_WorldViewProj, float4(input.Position, 1));
	
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

uniform mat4 _WorldViewProj;

in vec4 Position;

void main()
{
	gl_Position = _WorldViewProj * Position;
}

#endif
#ifdef FRAGMENT_SHADER

uniform vec4 _Ambient;

out vec4 vFragColor;

void main()
{
	vFragColor = clamp(_Ambient, 0.0, 1.0);
}

#endif
#endif
