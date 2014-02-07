#ifdef VERSION_DX

// Uniforms.
//
cbuffer Uniforms
{
	matrix _World;
	matrix _CubeVP[6];
}


// Vertex Shader.
//
struct VSInput
{
	float4 Position		:POSITION;
};

struct VSOutput
{
	float4 Position		:SV_POSITION;
};

VSOutput VS_Main( VSInput input )
{
	VSOutput output;

	// Position
	output.Position = mul(_World, input.Position);

	return output;
}

float4 PS_Main( VSOutput input ) : SV_Target
{	
	// Camera Direction
	float3 camPos = normalize(input.CameraPos);

	// Normal
	float3 N = normalize(input.Normal);
	 
	// Attenuation
	float4 color0 = GetColor(input.LightPos, camPos, N, _LightColor, _LightAttn);
	
	// Final fragment color
	return color0;
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

layout(triangles) in;
layout(triangle_strip, max_vertices=18) out;

in vec4 vPosition[];

out vec4 gPosition;

void main()
{
	for(gl_Layer=0; gl_Layer<6; ++gl_Layer)
	{
		for(int tri_vert=0; tri_vert<3; ++tri_vert)
		{
			gPosition = vPosition[tri_vert];
			gl_Position = _LightCubeMatrix[gl_Layer] * gPosition;
			EmitVertex();
		}
		EndPrimitive();
	}
}

#endif
#ifdef FRAGMENT_SHADER

uniform vec3 _LightPos;

in vec4 gPosition;

void main()
{
	gl_FragDepth = distance(gPosition.xyz, _LightPos) * 0.02;
}

#endif
#endif