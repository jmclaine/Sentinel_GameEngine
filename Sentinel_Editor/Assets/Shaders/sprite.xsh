#ifdef VERSION_DX

// Textures.
//
Texture2D    _Texture0	:register(t0);
SamplerState _Sampler0	:register(s0);


// Vertex Shader.
//
struct VSInput
{
	float4 QuadCoord0	:QUADCOORD0;
	float4 Color		:COLOR;
	matrix Matrix		:MATRIX;
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
	float2 TexCoord0	:TEXCOORD0;
	float4 Color		:COLOR;
};

[maxvertexcount(4)]
void GS_Main(point VSInput input[1], inout TriangleStream<GSOutput> TriStream)
{
	GSOutput output;
	output.Color = input[0].Color;
	
	// Top right.
	//
	output.Position = mul(input[0].Matrix, float4(1, 1, 0, 1));
	output.TexCoord0 = float2(input[0].QuadCoord0.z, input[0].QuadCoord0.y);
	TriStream.Append(output);

	// Top left.
	//
	output.Position = mul(input[0].Matrix, float4(-1, 1, 0, 1));
	output.TexCoord0 = float2(input[0].QuadCoord0.x, input[0].QuadCoord0.y);
	TriStream.Append(output);

	// Bottom right.
	//
	output.Position = mul(input[0].Matrix, float4(1, -1, 0, 1));
	output.TexCoord0 = float2(input[0].QuadCoord0.z, input[0].QuadCoord0.w);
	TriStream.Append(output);

	// Bottom left.
	//
	output.Position = mul(input[0].Matrix, float4(-1, -1, 0, 1));
	output.TexCoord0 = float2(input[0].QuadCoord0.x, input[0].QuadCoord0.w);
	TriStream.Append(output);
}


// Pixel Shader.
//
float4 PS_Main(GSOutput input):SV_Target
{
	return _Texture0.Sample(_Sampler0, input.TexCoord0) * input.Color;
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

#version 330

#ifdef VERTEX_SHADER

in vec4 QuadCoord0;
in vec4 Color;
in mat4 Matrix;

out vec4 gvQuadCoord0;
out vec4 gvColor;
out mat4 gvMatrix;

void main()
{
	// Texture color
	gvQuadCoord0 = QuadCoord0;

	// Vertex color
	gvColor = Color;

	// Matrix
	gvMatrix = Matrix;
}

#endif
#ifdef GEOMETRY_SHADER

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

in vec4 gvQuadCoord0[];
in vec4 gvColor[];
in mat4 gvMatrix[];

out vec2 vTexCoord0;
out vec4 vColor;

void main()
{
	vColor = gvColor[0];

	// Top right.
	//
	gl_Position = gvMatrix[0] * vec4(1, 1, 0, 1);
	vTexCoord0 = vec2(gvQuadCoord0[0].z, gvQuadCoord0[0].y);
	EmitVertex();

	// Top left.
	//
	vColor = gvColor[0];
	gl_Position = gvMatrix[0] * vec4(-1, 1, 0, 1);
	vTexCoord0 = vec2(gvQuadCoord0[0].x, gvQuadCoord0[0].y);
	EmitVertex();

	// Bottom right.
	//
	vColor = gvColor[0];
	gl_Position = gvMatrix[0] * vec4(1, -1, 0, 1);
	vTexCoord0 = vec2(gvQuadCoord0[0].z, gvQuadCoord0[0].w);
	EmitVertex();

	// Bottom left.
	//
	vColor = gvColor[0];
	gl_Position = gvMatrix[0] * vec4(-1, -1, 0, 1);
	vTexCoord0 = vec2(gvQuadCoord0[0].x, gvQuadCoord0[0].w);
	EmitVertex();
	
	EndPrimitive();
}

#endif
#ifdef FRAGMENT_SHADER

uniform sampler2D _Texture0;

in vec2 vTexCoord0;
in vec4 vColor;

out vec4 vFragColor;

void main()
{
	vFragColor = clamp(texture2D(_Texture0, vTexCoord0) * vColor, 0.0, 1.0);
}

#endif
#endif
