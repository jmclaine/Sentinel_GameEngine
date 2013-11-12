#ifdef VERSION_DX

Texture2D tex0;
SamplerState defss
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


// Vertex Shader.
//
struct VSInput
{
	float4 Texture0		:TEXCOORD0;
	float4 Color		:COLOR0;

	row_major float4x4 Matrix	:MATRIX;
};

struct GSOutput
{
	float4 Position		:SV_POSITION;
	float2 Texture0		:TEXCOORD0;
	float4 Color		:COLOR0;
};

VSInput MyVS(VSInput input)
{
	return input;
}


// Geometry Shader.
//
[maxvertexcount(4)]
void MyGS(point VSInput input[1], inout TriangleStream<GSOutput> TriStream)
{
	GSOutput v;
	v.Color = input[0].Color;
	
	// Top right.
	//
	v.Position = mul(float4(1, 1, 0, 1), input[0].Matrix);
	v.Texture0 = float2(input[0].Texture0.z, input[0].Texture0.y);
	TriStream.Append(v);

	// Top left.
	//
	v.Position = mul(float4(-1, 1, 0, 1), input[0].Matrix);
	v.Texture0 = float2(input[0].Texture0.x, input[0].Texture0.y);
	TriStream.Append(v);

	// Bottom right.
	//
	v.Position = mul(float4(1, -1, 0, 1), input[0].Matrix);
	v.Texture0 = float2(input[0].Texture0.z, input[0].Texture0.w);
	TriStream.Append(v);

	// Bottom left.
	//
	v.Position = mul(float4(-1, -1, 0, 1), input[0].Matrix);
	v.Texture0 = float2(input[0].Texture0.x, input[0].Texture0.w);
	TriStream.Append(v);
}


// Fragment Shader.
//
float4 MyPS(GSOutput input):SV_Target
{
	return tex0.Sample(defss, input.Texture0) * input.Color;
}

technique11 MyTechnique
{
    pass P0
    {
		SetVertexShader(CompileShader(vs_4_0, MyVS()));
		SetGeometryShader(CompileShader(gs_4_0, MyGS()));
		SetPixelShader(CompileShader(ps_4_0, MyPS()));
    }
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

#version 330

#ifdef VERTEX_SHADER

in vec4 aTexture0;
in vec4 aColor;
in mat4 aMatrix;

out vec4 gvTex0;
out vec4 gvColor;
out mat4 gvMatrix;

void main()
{
	// Texture color
	gvTex0 = aTexture0;

	// Vertex color
	gvColor = aColor;

	// Matrix
	gvMatrix = aMatrix;
}

#endif
#ifdef GEOMETRY_SHADER

in vec4 gvTex0[];
in vec4 gvColor[];
in mat4 gvMatrix[];

out vec2 vTex0;
out vec4 vColor;

//layout (points) in;
//layout (triangle_strip, max_vertices=4) out;

void main()
{
	vColor = gvColor[0];

	// Top right.
	//
	gl_Position = gvMatrix[0] * vec4(1, 1, 0, 1);
	vTex0 = vec2(gvTex0[0].z, gvTex0[0].y);
	EmitVertex();

	// Top left.
	//
	gl_Position = gvMatrix[0] * vec4(-1, 1, 0, 1);
	vTex0 = vec2(gvTex0[0].x, gvTex0[0].y);
	EmitVertex();

	// Bottom right.
	//
	gl_Position = gvMatrix[0] * vec4(1, -1, 0, 1);
	vTex0 = vec2(gvTex0[0].z, gvTex0[0].w);
	EmitVertex();

	// Bottom left.
	//
	gl_Position = gvMatrix[0] * vec4(-1, -1, 0, 1);
	vTex0 = vec2(gvTex0[0].x, gvTex0[0].w);
	EmitVertex();
	
	EndPrimitive();
}

#endif
#ifdef FRAGMENT_SHADER

uniform sampler2D tex0;

in vec2 vTex0;
in vec4 vColor;

out vec4 vFragColor;

void main()
{
	vFragColor = clamp(texture2D(tex0, vTex0) * vColor, 0.0, 1.0);
}

#endif
#endif
