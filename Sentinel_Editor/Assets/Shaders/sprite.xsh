#ifdef VERSION_DX

// Textures.
//
Texture2D    tex0		:register(t0);
SamplerState sampler0	:register(s0);


// Vertex Shader.
//
struct VSInput
{
	float4 Texture0		:TEXCOORD0;
	float4 Color		:COLOR0;

	float4x4 Matrix		:MATRIX;
};

VSInput VS_Main(VSInput input)
{
	return input;
}


// Geometry Shader.
//
struct GSOutput
{
	float4 Position		:SV_POSITION;
	float2 Texture0		:TEXCOORD0;
	float4 Color		:COLOR0;
};

[maxvertexcount(4)]
void GS_Main(point VSInput input[1], inout TriangleStream<GSOutput> TriStream)
{
	GSOutput output;
	output.Color = input[0].Color;
	
	// Top right.
	//
	output.Position = mul(input[0].Matrix, float4(1, 1, 0, 1));
	output.Texture0 = float2(input[0].Texture0.z, input[0].Texture0.y);
	TriStream.Append(output);

	// Top left.
	//
	output.Position = mul(input[0].Matrix, float4(-1, 1, 0, 1));
	output.Texture0 = float2(input[0].Texture0.x, input[0].Texture0.y);
	TriStream.Append(output);

	// Bottom right.
	//
	output.Position = mul(input[0].Matrix, float4(1, -1, 0, 1));
	output.Texture0 = float2(input[0].Texture0.z, input[0].Texture0.w);
	TriStream.Append(output);

	// Bottom left.
	//
	output.Position = mul(input[0].Matrix, float4(-1, -1, 0, 1));
	output.Texture0 = float2(input[0].Texture0.x, input[0].Texture0.w);
	TriStream.Append(output);
}


// Pixel Shader.
//
float4 PS_Main(GSOutput input):SV_Target
{
	return tex0.Sample(sampler0, input.Texture0) * input.Color;
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
