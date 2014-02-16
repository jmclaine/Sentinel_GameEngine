#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
	matrix _World;
	matrix _LightCubeMatrix[6];
	float3 _LightPos;
}


// Vertex Shader.
//
struct VSInput
{
	float3 Position		:POSITION;
};

struct VSOutput
{
	float4 Position		:SV_POSITION;
};

VSOutput VS_Main( VSInput input )
{
	VSOutput output;

	// Position
	output.Position = mul(_World, float4(input.Position, 1.0));

	return output;
}

// Geometry Shader.
//
struct GSOutput
{
	float4 Position		:SV_POSITION;
	float3 LightPos		:NORMAL;
	uint   RTIndex		:SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GS_Main( triangle VSOutput input[3], inout TriangleStream<GSOutput> stream )
{
	GSOutput output;

	for( int face = 0; face < 6; ++face )
    {
		output.RTIndex = face;

		for( int vert = 0; vert < 3; ++vert )
		{
			output.Position = mul(_LightCubeMatrix[face], input[vert].Position);
			output.LightPos = input[vert].Position.xyz - _LightPos;
			stream.Append( output );
		}
		stream.RestartStrip();
    }
}

// Pixel Shader.
//
const float shadowRadius = 1.0 / 25.0;

float PS_Main( GSOutput input ) : SV_Target
{
	return length(input.LightPos) * shadowRadius;
}

#endif
//////////////////////////////////////////////////////////////////////////////
#ifdef VERSION_GL

#version 330

#ifdef VERTEX_SHADER

uniform mat4 _World;

in vec3 Position;

out vec4 vPosition;

void main()
{
	vPosition = _World * vec4(Position, 1.0);
}

#endif
#ifdef GEOMETRY_SHADER

uniform mat4 _LightCubeMatrix[6];
uniform vec3 _LightPos;

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

in vec4 vPosition[];

out vec4 gLightPos;

void main()
{
	for( gl_Layer = 0; gl_Layer < 6; ++gl_Layer )
	{
		for( int vert = 0; vert < 3; ++vert )
		{
			gl_Position = _LightCubeMatrix[gl_Layer] * vPosition[vert];
			gLightPos = vec4(vPosition[vert].xyz - _LightPos, 1);
			EmitVertex();
		}
		EndPrimitive();
	}
}

#endif
#ifdef FRAGMENT_SHADER

in vec4 gLightPos;

const float shadowRadius = 25.0;

void main()
{
	float depth = length(gLightPos) / shadowRadius;

	gl_FragDepth = depth;
	/*
	float dx = dFdx(depth);
	float dy = dFdy(depth);

	gl_FragColor.rg = vec2(depth, depth*depth + 0.25*(dx*dx + dy*dy));
	//*/
}

#endif
#endif