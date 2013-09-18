uniform float4x4 wvp:WORLDVIEWPROJECTION;
uniform float4   texScale;

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
	float2   Texture0	:TEXCOORD0;
	float4   Color		:COLOR0;

	row_major float4x4 Matrix	:MATRIX;
};

struct VSOutput
{
	float4 Position		:SV_POSITION;
	float2 Texture0		:TEXCOORD0;
	float4 Color		:COLOR0;

	row_major float4x4 Matrix	:MATRIX;
};

VSOutput MyVS(VSInput input)
{
	VSOutput output;

	output.Position = float4(0, 0, 0, 1);
	output.Texture0 = input.Texture0 * texScale.xy;
	output.Color    = input.Color;

	output.Matrix   = mul(input.Matrix, wvp);

	return output;
}


// Geometry Shader.
//
[maxvertexcount(4)]
void MyGS(point VSOutput input[1], inout TriangleStream<VSOutput> TriStream)
{
	VSOutput v;
	v.Color  = input[0].Color;
	v.Matrix = input[0].Matrix;

	// Bottom left.
	//
	v.Position = mul(float4(-1, 1, 0, 1), input[0].Matrix);
	v.Texture0 = float2(input[0].Texture0.x, input[0].Texture0.y + texScale.y);
	TriStream.Append(v);

	// Bottom right.
	//
	v.Position = mul(float4(1, 1, 0, 1), input[0].Matrix);
	v.Texture0 = input[0].Texture0 + texScale.xy;
	TriStream.Append(v);

	// Top left.
	//
	v.Position = mul(float4(-1, -1, 0, 1), input[0].Matrix);
	v.Texture0 = input[0].Texture0;
	TriStream.Append(v);

	// Top right.
	//
	v.Position = mul(float4(1, -1, 0, 1), input[0].Matrix);
	v.Texture0 = float2(input[0].Texture0.x + texScale.x, input[0].Texture0.y);
	TriStream.Append(v);
}


// Fragment Shader.
//
float4 MyPS(VSOutput input):SV_Target
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