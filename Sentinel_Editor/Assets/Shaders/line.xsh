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
	float4 Position : POSITION;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
};

VSOutput VS_Main(VSInput input)
{
	VSOutput output;

	output.Position = mul(_WVP, input.Position);

	return output;
}


// Pixel Shader.
//
float4 PS_Main(VSOutput input) :SV_Target
{
	return saturate(_Ambient);
}

#endif

//////////////////////////////////////////////////////////////////////////////

#ifdef VERSION_GL
#ifdef VERTEX_SHADER

uniform mat4 _WVP;

in vec3 Position;

out vec4 gvPosition;

void main()
{
	gvPosition = _WVP * vec4(Position, 1);
}

#endif
#ifdef GEOMETRY_SHADER

layout(lines) in;
layout(triangle_strip, max_vertices = 6) out;

in vec4 gvPosition[2];

out vec4 vPosition;

void main()
{
	vec4 p0 = gvPosition[0];
	vec4 p1 = gvPosition[1];

	float w0 = p0.w;
	float w1 = p1.w;

	p0.xyz /= p0.w;
	p1.xyz /= p1.w;

	vec3 line01 = p1.xyz - p0.xyz;
	vec3 dir = normalize(line01);

	// scale to correct window aspect ratio
	//vec3 ratio = vec3(RenderTextureSize.y, RenderTextureSize.x, 0);
	//vec3 ratio = vec3(1440.0, 900.0, 0.0);
	//ratio = normalize(ratio);

	//vec3 normal = normalize(cross(unit_z, dir) * ratio);
	vec3 normal = normalize(cross(vec3(0, 0, -1), dir));

	float width = 0.001;

	//vec3 dir_offset = dir * ratio * width;
	//vec3 normal_scaled = normal * ratio * width;

	vec3 dir_offset = dir * width;
	vec3 normal_scaled = normal * width;

	vec3 p0_ex = p0.xyz - dir_offset;
	vec3 p1_ex = p1.xyz + dir_offset;

	vec4 v0 = vec4(p0_ex - normal_scaled, 1) * w0;
	vec4 v1 = vec4(p0_ex + normal_scaled, 1) * w0;
	vec4 v2 = vec4(p1_ex + normal_scaled, 1) * w1;
	vec4 v3 = vec4(p1_ex - normal_scaled, 1) * w1;

	gl_Position = v2;
	EmitVertex();

	gl_Position = v1;
	EmitVertex();

	gl_Position = v0;
	EmitVertex();

	EndPrimitive();

	gl_Position = v3;
	EmitVertex();

	gl_Position = v2;
	EmitVertex();

	gl_Position = v0;
	EmitVertex();

	EndPrimitive();
}

#endif
#ifdef FRAGMENT_SHADER

uniform vec4 _Ambient;

in vec4 vPosition;

out vec4 vFragColor;

void main()
{
	vFragColor = clamp(_Ambient, 0.0, 1.0);
}

#endif
#endif
