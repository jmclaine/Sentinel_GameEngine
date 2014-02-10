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
uniform vec3 _LightPos;

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
			gPosition = vPosition[tri_vert] - vec4(_LightPos, 1);
			gl_Position = _LightCubeMatrix[gl_Layer] * vPosition[tri_vert];
			EmitVertex();
		}
		EndPrimitive();
	}
}

#endif
#ifdef FRAGMENT_SHADER

uniform vec3 _LightPos;

in vec4 gPosition;

const float shadowRadius = 1.0 / 200.0;

void main()
{
	float depth = length(gPosition) * shadowRadius;
	//gl_FragColor.r = min(depth, gl_FragCoord.z);
	//gl_FragDepth = max(depth, gl_FragDepth);
	gl_FragDepth = depth;
	//gl_FragColor.r = depth;
	//gl_FragColor.g = depth*depth;
}

#endif
#endif