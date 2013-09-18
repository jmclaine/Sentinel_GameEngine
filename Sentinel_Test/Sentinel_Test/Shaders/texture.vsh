uniform mat4 wvp;
uniform mat4 world;

uniform vec3 light_pos0;
uniform vec3 cam_pos;

attribute vec4 aPosition;
attribute vec2 aTexture0;
attribute vec3 aNormal;

varying vec2 vTexture0;
varying vec3 vNormal;
varying vec3 vCamDir;
varying vec3 vLPos0;

void main()
{
	gl_Position   = mul(wvp, aPosition);
	vec3 worldPos = mul(world, aPosition);

	// Light direction
	vLPos0  = light_pos0 - worldPos;
	
	// Camera direction
	vCamDir = cam_pos - worldPos;

	// Texture
	vTexture0 = aTexture0;

	// Normal
	vNormal = mul(world, vec4(aNormal, 0));
}
