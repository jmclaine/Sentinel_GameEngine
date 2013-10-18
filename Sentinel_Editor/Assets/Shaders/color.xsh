#ifdef VERSION_DX

// Uniforms.
//
uniform matrix wvp  :WORLDVIEWPROJECTION;


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

VSOutput MyVS( VSInput input )
{
	VSOutput output;

	output.Position = mul(input.Position, wvp);
	
	return output;
}


// Fragment Uniforms.
//
uniform float4 ambient;
uniform float4 diffuse;
uniform float4 specular;
uniform float spec_comp;

float4 MyPS(VSOutput input):SV_Target
{	
	return saturate(ambient + diffuse + specular);
}

technique11 MyTechnique
{
    pass P0
    {          
		SetVertexShader(CompileShader(vs_4_0, MyVS()));
		SetGeometryShader(0);
		SetPixelShader(CompileShader(ps_4_0, MyPS()));
    }
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
   gl_FragColor = clamp(ambient + diffuse + specular, 0.0, 1.0);
}

#endif
#endif
