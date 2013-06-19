uniform mat4 mvp;
uniform mat4 world;

uniform vec3 cam_pos;
uniform vec3 light_pos0;

attribute vec4 aPosition;
attribute vec2 aTexture0;
attribute vec3 aNormal;
attribute vec4 aTangent;

varying vec2 vTexture0;
varying vec3 vNormal;
varying vec3 vCamDir;
varying vec3 vLPos0;
varying vec3 vLDist0;

void main()
{
	gl_Position   = mul(mvp, aPosition);
	vec3 worldPos = mul(world, aPosition).xyz;

	// Transform light direction into tangent space.
	//
	vec3 normal   = mul(world, vec4(aNormal.xyz, 0)).xyz;
	vec3 tangent  = mul(world, vec4(aTangent.xyz, 0)).xyz;
	vec3 binormal = cross(normal, tangent.xyz) * aTangent.w;

	mat3 matTBN = mat3(tangent.x, binormal.x, normal.x, 
					   tangent.y, binormal.y, normal.y, 
					   tangent.z, binormal.z, normal.z);

	// Camera direction
	vCamDir = mul(matTBN, cam_pos - worldPos);

	// Light Position
	vec3 LPos = light_pos0 - worldPos;
	vLPos0    = mul(matTBN, LPos);
	vLDist0   = LPos;

	// Texture color
	vTexture0 = aTexture0;
}