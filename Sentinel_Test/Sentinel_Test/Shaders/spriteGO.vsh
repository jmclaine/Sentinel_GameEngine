#version 330 

uniform mat4 mvp;
uniform vec4 texScale;

in vec2 aTexture0;
in vec4 aColor;
in mat4 aMatrix;

out vec2 gvTex0;
out vec4 gvColor;
out mat4 gvMatrix;

void main()
{
	// Position
	gl_Position = vec4(0, 0, 0, 1);
	
	// Texture color
	gvTex0 = aTexture0 * texScale.xy;

	// Vertex color
	gvColor = aColor;

	// Matrix
	gvMatrix = mvp * aMatrix;
}