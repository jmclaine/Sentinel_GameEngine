#version 330 

uniform vec4 texScale;

in vec2 gvTex0[];
in vec4 gvColor[];
in mat4 gvMatrix[];

out vec2 vTex0;
out vec4 vColor;

layout (points) in;
layout (triangle_strip, max_vertices=4) out;

void main()
{
	vColor = gvColor[0];

	// Bottom left.
	//
	gl_Position = gvMatrix[0] * vec4(-1, 1, 0, 1);
	vTex0 = vec2(gvTex0[0].x, gvTex0[0].y + texScale.y);
	EmitVertex();

	// Bottom right.
	//
	gl_Position = gvMatrix[0] * vec4(1, 1, 0, 1);
	vTex0 = gvTex0[0] + texScale.xy;
	EmitVertex();

	// Top left.
	//
	gl_Position = gvMatrix[0] * vec4(-1, -1, 0, 1);
	vTex0 = gvTex0[0];
	EmitVertex();

	// Top right.
	//
	gl_Position = gvMatrix[0] * vec4(1, -1, 0, 1);
	vTex0 = vec2(gvTex0[0].x + texScale.x, gvTex0[0].y);
	EmitVertex();
	
	EndPrimitive();
}