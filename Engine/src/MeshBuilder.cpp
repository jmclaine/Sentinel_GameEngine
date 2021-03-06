#include <vector>
#include <memory>

#include "MeshBuilder.h"
#include "Renderer.h"
#include "MathUtil.h"
#include "Mesh.h"
#include "Shader.h"
#include "Buffer.h"
#include "Texture.h"
#include "VertexLayout.h"

namespace Sentinel
{
	MeshBuilder::MeshBuilder()
	{
		ClearAll();
	}

	MeshBuilder::~MeshBuilder()
	{ }

	void MeshBuilder::ClearAll()
	{
		mPrimitive = PrimitiveFormat::TRIANGLES;

		ClearGeometry();
	}

	void MeshBuilder::ClearGeometry()
	{
		mVertexBuffer = nullptr;
		mIndexBuffer = nullptr;

		mVertex.clear();
		mIndex.clear();
	}

	//////////////////////////////////////////////////////////////////

	UINT MeshBuilder::FindVertex(const Vector3& pos, const Vector2& tex, const Vector3& normal)
	{
		for (UINT i = 0; i < mVertex.size(); ++i)
		{
			if (mVertex[i].mPosition == pos &&
				mVertex[i].mTexCoord[0] == tex &&
				mVertex[i].mNormal == normal)
			{
				return i;
			}
		}

		return UINT_MAX; // Vertex not found.
	}

	//////////////////////////////////////////////////////////////////

	void MeshBuilder::AddIndex(UINT i0)
	{
		mIndex.push_back(i0);
	}

	void MeshBuilder::AddIndex(UINT i0, UINT i1)
	{
		AddIndex(i0);
		AddIndex(i1);
	}

	void MeshBuilder::AddIndex(UINT i0, UINT i1, UINT i2)
	{
		AddIndex(i0);
		AddIndex(i1);
		AddIndex(i2);
	}

	void MeshBuilder::AddIndex(UINT i0, UINT i1, UINT i2, UINT i3)
	{
		AddIndex(i0, i1, i2);
		AddIndex(i0, i2, i3);
	}

	void MeshBuilder::AddIndex(UINT i0, UINT i1, UINT i2, UINT i3, UINT i4)
	{
		AddIndex(i0, i1, i2);
		AddIndex(i0, i2, i3);
		AddIndex(i0, i3, i4);
	}

	////////////////////////////////////////////////////

	void MeshBuilder::CalculateTangents(bool doNormals)
	{
		Vector3* tangent = new Vector3[mVertex.size()];
		Vector3* bitangent = new Vector3[mVertex.size()];
		Vector3* normal = new Vector3[mVertex.size()];

		for (UINT i = 0; i < mIndex.size(); i += 3)
		{
			UINT j[3] = { mIndex[i], mIndex[i + 1], mIndex[i + 2] };

			Vector3 Q1 = mVertex[j[1]].mPosition - mVertex[j[0]].mPosition;
			Vector3 Q2 = mVertex[j[2]].mPosition - mVertex[j[0]].mPosition;

			Vector3 N;
			if (doNormals)
			{
				N = (Q1.Cross(Q2)).Normalize();

				mVertex[j[0]].mNormal = N;
				mVertex[j[1]].mNormal = N;
				mVertex[j[2]].mNormal = N;
			}
			else
			{
				N = (mVertex[j[0]].mNormal).Normalize();
			}

			// Calculate the tangents.
			//
			float s1 = mVertex[j[1]].mTexCoord[0].x - mVertex[j[0]].mTexCoord[0].x;
			float t1 = mVertex[j[1]].mTexCoord[0].y - mVertex[j[0]].mTexCoord[0].y;

			float s2 = mVertex[j[2]].mTexCoord[0].x - mVertex[j[0]].mTexCoord[0].x;
			float t2 = mVertex[j[2]].mTexCoord[0].y - mVertex[j[0]].mTexCoord[0].y;

			float det = 1.0f / (s1*t2 - s2*t1);

			Vector3 T = Vector3(
				(Q1.x * t2 - Q2.x * t1) * det,
				(Q1.y * t2 - Q2.y * t1) * det,
				(Q1.z * t2 - Q2.z * t1) * det);

			Vector3 B = Vector3(
				(Q2.x * s1 - Q1.x * s2) * det,
				(Q2.y * s1 - Q1.y * s2) * det,
				(Q2.z * s1 - Q1.z * s2) * det);

			// Sum the tangents up by their vertex for averaging.
			//
			for (UINT k = 0; k < 3; ++k)
			{
				tangent[j[k]] += T;
				bitangent[j[k]] += B;
				normal[j[k]] += N;
			}
		}

		for (UINT i = 0; i < mVertex.size(); ++i)
		{
			// normalize only for vertex correctness
			Vector3 N(normal[i].Normalize());
			Vector3 T(tangent[i].Normalize());
			Vector3 B(bitangent[i].Normalize());

			Vector3 Tn = (T - N * N.Dot(T)).Normalize();
			float sign = ((N.Cross(T)).Dot(B) < 0.0f) ? -1.0f : 1.0f;

			mVertex[i].mTangent = Vector4(Tn.x, Tn.y, Tn.z, sign);
		}

		delete[] tangent;
		delete[] bitangent;
		delete[] normal;
	}

	//////////////////////////////////////////////////////////////////

	void MeshBuilder::CreateLine(const Vector3& start, const Vector3& end)
	{
		UINT startVert = mVertex.size();

		Vertex vertex;

		vertex.mPosition = start;

		mVertex.push_back(vertex);

		vertex.mPosition = end;

		mVertex.push_back(vertex);

		AddIndex(startVert, startVert + 1);
	}

	void MeshBuilder::CreateQuad(float size, const Vector3& normal)
	{
		UINT startVert = mVertex.size();

		Vector3 up = Vector3(normal.y, normal.z, normal.x);
		Vector3 right = normal.Cross(up);

		Vector3 cornerUR = right + up;
		Vector3 cornerUL = up - right;

		Vertex vertex;
		vertex.mPosition = cornerUL * -size;
		vertex.mTexCoord[0] = Vector2(0, 1);
		vertex.mTexCoord[1] = Vector2(0, 1);
		vertex.mNormal = normal;

		mVertex.push_back(vertex);

		vertex.mPosition = cornerUR * -size;
		vertex.mTexCoord[0] = Vector2(1, 1);
		vertex.mTexCoord[1] = Vector2(1, 1);
		vertex.mNormal = normal;

		mVertex.push_back(vertex);

		vertex.mPosition = cornerUL * size;
		vertex.mTexCoord[0] = Vector2(1, 0);
		vertex.mTexCoord[1] = Vector2(1, 0);
		vertex.mNormal = normal;

		mVertex.push_back(vertex);

		vertex.mPosition = cornerUR * size;
		vertex.mTexCoord[0] = Vector2(0, 0);
		vertex.mTexCoord[1] = Vector2(0, 0);
		vertex.mNormal = normal;

		mVertex.push_back(vertex);

		AddIndex(startVert, startVert + 1, startVert + 2, startVert + 3);

		mPrimitive = PrimitiveFormat::TRIANGLES;
	}

#define CUBE_SIDE( n )\
	normal = n;\
	startVert = mVertex.size();\
	matTrans.Translate(normal * size);\
	CreateQuad(size, normal);\
	ApplyMatrix(matTrans, startVert);

	void MeshBuilder::CreateCube(float size)
	{
		Matrix4x4 matTrans;
		Vector3 normal;
		UINT startVert;

		// Front
		CUBE_SIDE(Vector3(0, 0, -1));

		// Left
		CUBE_SIDE(Vector3(-1, 0, 0));

		// Bottom
		CUBE_SIDE(Vector3(0, -1, 0));

		// Back
		CUBE_SIDE(Vector3(0, 0, 1));

		// Right
		CUBE_SIDE(Vector3(1, 0, 0));

		// Top
		CUBE_SIDE(Vector3(0, 1, 0));
	}

	void MeshBuilder::CreateWireCube(float size)
	{
		UINT startVert = mVertex.size();

		Vertex vertex;

		// Left Bottom Back (0)
		vertex.mPosition = Vector3(-size, -size, -size);
		mVertex.push_back(vertex);

		// Right Bottom Back (1)
		vertex.mPosition = Vector3(size, -size, -size);
		mVertex.push_back(vertex);

		// Left Top Back (2)
		vertex.mPosition = Vector3(-size, size, -size);
		mVertex.push_back(vertex);

		// Right Top Back (3)
		vertex.mPosition = Vector3(size, size, -size);
		mVertex.push_back(vertex);

		// Left Bottom Front (4)
		vertex.mPosition = Vector3(-size, -size, size);
		mVertex.push_back(vertex);

		// Right Bottom Front (5)
		vertex.mPosition = Vector3(size, -size, size);
		mVertex.push_back(vertex);

		// Left Top Front (6)
		vertex.mPosition = Vector3(-size, size, size);
		mVertex.push_back(vertex);

		// Right Top Front (7)
		vertex.mPosition = Vector3(size, size, size);
		mVertex.push_back(vertex);

		// Back
		AddIndex(startVert, startVert + 1);
		AddIndex(startVert, startVert + 2);
		AddIndex(startVert + 1, startVert + 3);
		AddIndex(startVert + 2, startVert + 3);

		// Top
		//AddIndex( startVert+2, startVert+3 );
		AddIndex(startVert + 2, startVert + 6);
		AddIndex(startVert + 3, startVert + 7);
		AddIndex(startVert + 6, startVert + 7);

		// Left
		//AddIndex( startVert, startVert+2 );
		AddIndex(startVert, startVert + 4);
		//AddIndex( startVert+2, startVert+6 );
		AddIndex(startVert + 4, startVert + 6);

		// Front
		AddIndex(startVert + 4, startVert + 5);
		//AddIndex( startVert+6, startVert+7 );
		//AddIndex( startVert+4, startVert+6 );
		AddIndex(startVert + 5, startVert + 7);

		// Bottom
		//AddIndex( startVert, startVert+1 );
		//AddIndex( startVert+4, startVert+5 );
		AddIndex(startVert + 1, startVert + 5);
		//AddIndex( startVert, startVert+4 );

		// Right
		//AddIndex( startVert+1, startVert+3 );
		//AddIndex( startVert+5, startVert+7 );
		//AddIndex( startVert+1, startVert+5 );
		//AddIndex( startVert+3, startVert+7 );

		mPrimitive = PrimitiveFormat::LINES;
	}

	// Shape code base from geometry.h of OpenGL.
	// Creates a lookup table for drawing a circle.
	//
	void CircleTable(float **sint, float **cost, const int n)
	{
		int i;

		/* Table size, the sign of n flips the circle direction */

		const int size = abs(n);

		/* Determine the angle between samples */

		const float angle = (float)(2 * PI) / (float)n;

		/* Allocate memory for n samples, plus duplicate of first entry at the end */

		*sint = (float*)calloc(sizeof(double), size + 1);
		*cost = (float*)calloc(sizeof(double), size + 1);

		/* Bail out if memory allocation fails */

		if (!(*sint) || !(*cost))
		{
			free(*sint);
			free(*cost);
		}

		/* Compute cos and sin around the circle */

		for (i = 0; i < size; i++)
		{
			(*sint)[i] = ::sin(angle*i);
			(*cost)[i] = ::cos(angle*i);
		}

		/* Last sample is duplicate of the first */

		(*sint)[size] = (*sint)[0];
		(*cost)[size] = (*cost)[0];
	}

	void MeshBuilder::CreateCylinder(float radius, float height, int slices, int stacks)
	{
		int i, j;

		/* Step in z and radius as stacks are drawn. */

		float z0, z1;
		const float zStep = height / stacks;
		const float zHeight = height / 2.0f;

		/* Pre-computed circle */

		float *sint, *cost;
		CircleTable(&sint, &cost, -slices);

		/* Cover the base and top */

		UINT startVertex = mVertex.size();
		int vertCount = (int)startVertex;

		Vertex vertex;
		vertex.mPosition = Vector3(0.0f, -zHeight, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.5f, 0.5f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.5f);
		vertex.mNormal = Vector3(0.0f, -1.0f, 0.0f);

		mVertex.push_back(vertex);

		for (j = 0; j <= slices; j++)
		{
			vertex.mPosition = Vector3(cost[j] * radius, -zHeight, sint[j] * radius);
			vertex.mTexCoord[0] = Vector2(0.5f + cost[j] * 0.5f, 0.5f + sint[j] * 0.5f);
			vertex.mTexCoord[1] = Vector2(0.5f + cost[j] * 0.5f, 0.5f + sint[j] * 0.5f);

			mVertex.push_back(vertex);
		}

		for (j = vertCount; j < (int)mVertex.size() - 2; j++)
		{
			AddIndex(vertCount, j + 2, j + 1);
		}

		/////////////////////////////

		vertCount = (int)mVertex.size();

		vertex.mPosition = Vector3(0.0f, zHeight, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.5f, 0.5f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.5f);
		vertex.mNormal = Vector3(0.0f, 1.0f, 0.0f);

		mVertex.push_back(vertex);

		for (j = slices; j >= 0; j--)
		{
			vertex.mPosition = Vector3(cost[j] * radius, zHeight, sint[j] * radius);
			vertex.mTexCoord[0] = Vector2(0.5f + cost[j] * 0.5f, 0.5f + sint[j] * 0.5f);
			vertex.mTexCoord[1] = Vector2(0.5f + cost[j] * 0.5f, 0.5f + sint[j] * 0.5f);

			mVertex.push_back(vertex);
		}

		for (j = vertCount; j < (int)mVertex.size() - 2; j++)
		{
			AddIndex(vertCount, j + 2, j + 1);
		}

		/* Do the stacks */

		z0 = -zHeight;
		z1 = zStep;

		for (i = 1; i <= stacks; i++)
		{
			if (i == stacks)
			{
				z1 = zHeight;
			}

			vertCount = (int)mVertex.size();

			for (j = 0; j <= slices; j++)
			{
				float u = ((float)j / (float)slices * 2.0f);

				vertex.mPosition = Vector3(cost[j] * radius, z0, sint[j] * radius);
				vertex.mTexCoord[0] = Vector2(u, 0.0f);
				vertex.mTexCoord[1] = Vector2(u, 0.0f);
				vertex.mNormal = Vector3(cost[j], 0.0f, sint[j]);

				mVertex.push_back(vertex);

				vertex.mPosition = Vector3(cost[j] * radius, z1, sint[j] * radius);
				vertex.mTexCoord[0] = Vector2(u, 1.0f);
				vertex.mTexCoord[1] = Vector2(u, 1.0f);
				vertex.mNormal = Vector3(cost[j], 0.0f, sint[j]);

				mVertex.push_back(vertex);
			}

			for (j = vertCount; j < (int)mVertex.size() - 3; j += 2)
			{
				AddIndex(j, j + 2, j + 3, j + 1);
			}

			z0 = z1;
			z1 += zStep;
		}

		/* Release sin and cos tables */

		free(sint);
		free(cost);

		mPrimitive = PrimitiveFormat::TRIANGLES;
	}

	void MeshBuilder::CreateTetrahedron(float scale)
	{
		/* Magic Numbers:  r0 = ( 1, 0, 0 )
		*                  r1 = ( -1/3, 2 sqrt(2) / 3, 0 )
		*                  r2 = ( -1/3, -sqrt(2) / 3, sqrt(6) / 3 )
		*                  r3 = ( -1/3, -sqrt(2) / 3, -sqrt(6) / 3 )
		* |r0| = |r1| = |r2| = |r3| = 1
		* Distance between any two points is 2 sqrt(6) / 3
		*
		* Normals:  The unit normals are simply the negative of the coordinates of the point not on the surface.
		*/

		Vector3 r0(1.0f, 0.0f, 0.0f);
		Vector3 r1(-0.333333333333f, 0.942809041582f, 0.0f);
		Vector3 r2(-0.333333333333f, -0.471404520791f, 0.816496580928f);
		Vector3 r3(-0.333333333333f, -0.471404520791f, -0.816496580928f);

		scale *= 1.25f;
		r0 *= scale;
		r1 *= scale;
		r2 *= scale;
		r3 *= scale;

		UINT numVerts = mVertex.size();

		////////////////////////////////////////

		Vertex vertex;
		vertex.mPosition = r1;
		vertex.mTexCoord[0] = Vector2(0.5f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 1.0f);
		vertex.mNormal = Vector3(-1.0f, 0.0f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = r3;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(-1.0f, 0.0f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = r2;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(-1.0f, 0.0f, 0.0f);

		mVertex.push_back(vertex);

		////////////////////////////////////////

		vertex.mPosition = r0;
		vertex.mTexCoord[0] = Vector2(0.5f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 1.0f);
		vertex.mNormal = Vector3(0.333333333333f, -0.942809041582f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = r2;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(0.333333333333f, -0.942809041582f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = r3;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(0.333333333333f, -0.942809041582f, 0.0f);

		mVertex.push_back(vertex);

		////////////////////////////////////////

		vertex.mPosition = r0;
		vertex.mTexCoord[0] = Vector2(0.5f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 1.0f);
		vertex.mNormal = Vector3(0.333333333333f, 0.471404520791f, -0.816496580928f);

		mVertex.push_back(vertex);

		vertex.mPosition = r3;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(0.333333333333f, 0.471404520791f, -0.816496580928f);

		mVertex.push_back(vertex);

		vertex.mPosition = r1;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(0.333333333333f, 0.471404520791f, -0.816496580928f);

		mVertex.push_back(vertex);

		////////////////////////////////////////

		vertex.mPosition = r0;
		vertex.mTexCoord[0] = Vector2(0.5f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 1.0f);
		vertex.mNormal = Vector3(0.333333333333f, 0.471404520791f, 0.816496580928f);

		mVertex.push_back(vertex);

		vertex.mPosition = r1;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(0.333333333333f, 0.471404520791f, 0.816496580928f);

		mVertex.push_back(vertex);

		vertex.mPosition = r2;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(0.333333333333f, 0.471404520791f, 0.816496580928f);

		mVertex.push_back(vertex);

		////////////////////////////////////////

		for (UINT x = numVerts; x < mVertex.size(); x += 3)
		{
			AddIndex(x, x + 1, x + 2);
		}

		mPrimitive = PrimitiveFormat::TRIANGLES;
	}

	void MeshBuilder::CreateOctahedron(float radius)
	{
		UINT numVerts = mVertex.size();

		////////////////////////////////////////

		Vertex vertex;
		vertex.mPosition = Vector3(radius, 0.0f, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(0.577350269189f, 0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, radius, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 1.0f);
		vertex.mNormal = Vector3(0.577350269189f, 0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 0.0f, radius);
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(0.577350269189f, 0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		////////////////////////////////////////

		vertex.mPosition = Vector3(radius, 0.0f, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(0.577350269189f, 0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 0.0f, -radius);
		vertex.mTexCoord[0] = Vector2(0.0f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 1.0f);
		vertex.mNormal = Vector3(0.577350269189f, 0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, radius, 0.0f);
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(0.577350269189f, 0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		//////////////////////////////////////////

		vertex.mPosition = Vector3(radius, 0.0f, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(0.577350269189f, -0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 0.0f, radius);
		vertex.mTexCoord[0] = Vector2(0.0f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 1.0f);
		vertex.mNormal = Vector3(0.577350269189f, -0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, -radius, 0.0f);
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(0.577350269189f, -0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		//////////////////////////////////////////

		vertex.mPosition = Vector3(radius, 0.0f, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(0.577350269189f, -0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, -radius, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 1.0f);
		vertex.mNormal = Vector3(0.577350269189f, -0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 0.0f, -radius);
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(0.577350269189f, -0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		//////////////////////////////////////////

		vertex.mPosition = Vector3(-radius, 0.0f, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(-0.577350269189f, 0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 0.0f, radius);
		vertex.mTexCoord[0] = Vector2(0.0f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 1.0f);
		vertex.mNormal = Vector3(-0.577350269189f, 0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, radius, 0.0f);
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(-0.577350269189f, 0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		//////////////////////////////////////////

		vertex.mPosition = Vector3(-radius, 0.0f, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(-0.577350269189f, 0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, radius, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 1.0f);
		vertex.mNormal = Vector3(-0.577350269189f, 0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 0.0f, -radius);
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(-0.577350269189f, 0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		//////////////////////////////////////////

		vertex.mPosition = Vector3(-radius, 0.0f, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(-0.577350269189f, -0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, -radius, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 1.0f);
		vertex.mNormal = Vector3(-0.577350269189f, -0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 0.0f, radius);
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(-0.577350269189f, -0.577350269189f, 0.577350269189f);

		mVertex.push_back(vertex);

		//////////////////////////////////////////

		vertex.mPosition = Vector3(-radius, 0.0f, 0.0f);
		vertex.mTexCoord[0] = Vector2(0.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.0f);
		vertex.mNormal = Vector3(-0.577350269189f, -0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 0.0f, -radius);
		vertex.mTexCoord[0] = Vector2(0.0f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.0f, 1.0f);
		vertex.mNormal = Vector3(-0.577350269189f, -0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, -radius, 0.0f);
		vertex.mTexCoord[0] = Vector2(1.0f, 0.0f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.0f);
		vertex.mNormal = Vector3(-0.577350269189f, -0.577350269189f, -0.577350269189f);

		mVertex.push_back(vertex);

		////////////////////////////////////////

		for (UINT x = numVerts; x < mVertex.size(); x += 3)
		{
			AddIndex(x, x + 1, x + 2);
		}

		mPrimitive = PrimitiveFormat::TRIANGLES;
	}

	void MeshBuilder::CreateDodecahedron(float scale)
	{
		UINT numVerts = mVertex.size();

		scale *= 0.625f;

		Vertex vertex;
		vertex.mPosition = Vector3(0.0f, 1.61803398875f, 0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, 1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-0.61803398875f, 0.0f, 1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.61803398875f, 0.0f, 1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, 1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(0.0f, 1.61803398875f, -0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, 1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.61803398875f, 0.0f, -1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-0.61803398875f, 0.0f, -1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, 1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(0.0f, 0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(0.0f, -1.61803398875f, 0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, -1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.61803398875f, 0.0f, 1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-0.61803398875f, 0.0f, 1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, -1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, 0.850650808354f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(0.0f, -1.61803398875f, -0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, -1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-0.61803398875f, 0.0f, -1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.61803398875f, 0.0f, -1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, -1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(0.0f, -0.525731112119f, -0.850650808354f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(0.61803398875f, 0.0f, 1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, -1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.61803398875f, -0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.61803398875f, 0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, 1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(-0.61803398875f, 0.0f, 1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, 1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.61803398875f, 0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.61803398875f, -0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, -1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, 0.525731112119f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(0.61803398875f, 0.0f, -1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, 1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.61803398875f, 0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.61803398875f, -0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, -1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(-0.61803398875f, 0.0f, -1.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, -1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.61803398875f, -0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.61803398875f, 0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, 1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(-0.850650808354f, 0.0f, -0.525731112119f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(1.61803398875f, 0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, 1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 1.61803398875f, -0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 1.61803398875f, 0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, 1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(1.61803398875f, -0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, -1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, -1.61803398875f, 0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, -1.61803398875f, -0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(1.0f, -1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(-1.61803398875f, 0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(-0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, 1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(-0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 1.61803398875f, 0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(-0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, 1.61803398875f, -0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(-0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, 1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(-0.525731112119f, 0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		vertex.mPosition = Vector3(-1.61803398875f, -0.61803398875f, 0.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.5f, 0.0f);
		vertex.mTexCoord[1] = Vector2(0.5f, 0.0f);
		vertex.mNormal = Vector3(-0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, -1.0f, -1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(0.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(0.0f, 0.31f);
		vertex.mNormal = Vector3(-0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, -1.61803398875f, -0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.31f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.31f, 1.0f);
		vertex.mNormal = Vector3(-0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(0.0f, -1.61803398875f, 0.61803398875f) * scale;
		vertex.mTexCoord[0] = Vector2(0.69f, 1.0f);
		vertex.mTexCoord[1] = Vector2(0.69f, 1.0f);
		vertex.mNormal = Vector3(-0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		vertex.mPosition = Vector3(-1.0f, -1.0f, 1.0f) * scale;
		vertex.mTexCoord[0] = Vector2(1.0f, 0.31f);
		vertex.mTexCoord[1] = Vector2(1.0f, 0.31f);
		vertex.mNormal = Vector3(-0.525731112119f, -0.850650808354f, 0.0f);

		mVertex.push_back(vertex);

		/////////////////////////////////////////////////////////////////

		for (UINT x = numVerts; x < mVertex.size(); x += 5)
		{
			AddIndex(x, x + 1, x + 2, x + 3, x + 4);
		}

		mPrimitive = PrimitiveFormat::TRIANGLES;
	}

	void MeshBuilder::CreateWireSphere(float radius, int slices, int stacks)
	{
		Vertex vertex;

		int i, j;

		/* Adjust z and radius as stacks and slices are drawn. */

		float r;
		float x, y, z;

		/* Pre-computed circle */

		float *sint1, *cost1;
		float *sint2, *cost2;
		CircleTable(&sint1, &cost1, -slices);
		CircleTable(&sint2, &cost2, stacks << 1);

		/* Draw a line loop for each stack */

		for (i = 1; i < stacks; i++)
		{
			int vertCount = mVertex.size();
			z = cost2[i];
			r = sint2[i];

			for (j = 0; j <= slices; j++)
			{
				x = cost1[j];
				y = sint1[j];

				vertex.mPosition = Vector3(x*r*radius, y*r*radius, z*radius);
				vertex.mNormal = Vector3(x, y, z);

				mVertex.push_back(vertex);
			}

			vertex.mPosition = mVertex[0].mPosition;

			mVertex.push_back(vertex);

			for (j = vertCount; j < (int)mVertex.size() - 2; j++)
			{
				AddIndex(j, j + 1);
			}
		}

		/* Draw a line loop for each slice */

		for (i = 0; i < slices; i++)
		{
			int vertCount = (int)mVertex.size();

			for (j = 0; j <= stacks; j++)
			{
				x = cost1[i] * sint2[j];
				y = sint1[i] * sint2[j];
				z = cost2[j];

				vertex.mPosition = Vector3(x*radius, y*radius, z*radius);
				vertex.mNormal = Vector3(x, y, z);

				mVertex.push_back(vertex);
			}

			for (j = vertCount; j < (int)mVertex.size() - 1; j++)
			{
				AddIndex(j, j + 1);
			}
		}

		/* Release sin and cos tables */

		free(sint1);
		free(cost1);
		free(sint2);
		free(cost2);

		mPrimitive = PrimitiveFormat::LINES;
	}

	void MeshBuilder::CreateSphere(float radius, int slices, int stacks, int texWrap)
	{
		int i, j;

		/* Adjust z and radius as stacks are drawn. */

		float z0, z1;
		float r0, r1;

		/* Pre-computed circle */

		float *sint1, *cost1;
		float *sint2, *cost2;
		CircleTable(&sint1, &cost1, -slices);
		CircleTable(&sint2, &cost2, stacks << 1);

		float u, v;
		float tx, ty;
		float tx1, ty1;

		/* The top stack is covered with a triangle fan */

		z0 = 1.0; z1 = cost2[1];
		r0 = 0.0; r1 = sint2[1];

		int vertCount = (int)mVertex.size();

		Vertex vertex;
		vertex.mPosition = Vector3(0.0f, 0.0f, radius);
		vertex.mNormal = Vector3(0.0f, 0.0f, 1.0f);

		tx1 = 0.0f; //(atan2(0.0f, 1.0f) / (2.0f * (float)PI) + 0.5f) * texWrap;
		ty1 = 0.0f; //(acos(0.0f) / (float)PI + 0.5f) * texWrap;
		vertex.mTexCoord[0] = Vector2(tx1, ty1);
		vertex.mTexCoord[1] = Vector2(tx1, ty1);

		mVertex.push_back(vertex);

		for (j = slices; j >= 0; j--)
		{
			u = cost1[j] * r1;
			v = sint1[j] * r1;
			vertex.mPosition = Vector3(u*radius, v*radius, z1*radius);
			vertex.mNormal = Vector3(u, v, z1);

			tx1 = u * texWrap; //(atan2(u, z1) / (2.0f * (float)PI) + 0.5f) * texWrap;
			ty1 = v * texWrap; //(acos(v) / (float)PI + 0.5f) * texWrap;
			vertex.mTexCoord[0] = Vector2(tx1, ty1);
			vertex.mTexCoord[1] = Vector2(tx1, ty1);

			mVertex.push_back(vertex);
		}

		for (j = vertCount; j < (int)mVertex.size() - 2; j++)
		{
			AddIndex(vertCount, j + 1, j + 2);
		}

		/* Cover each stack with a quad strip, except the top and bottom stacks */

		for (i = 1; i < stacks - 1; i++)
		{
			vertCount = (int)mVertex.size();

			z0 = z1; z1 = cost2[i + 1];
			r0 = r1; r1 = sint2[i + 1];

			for (j = 0; j <= slices; j++)
			{
				u = cost1[j] * r1;
				v = sint1[j] * r1;

				vertex.mPosition = Vector3(u*radius, v*radius, z1*radius);
				vertex.mNormal = Vector3(u, v, z1);

				tx = u * texWrap; //(atan2(u, z1) / (2.0f * (float)PI) + 0.5f) * texWrap;
				ty = v * texWrap; //(acos(v) / (float)PI + 0.5f) * texWrap;
				//clamp( tx, -2.0f*(float)PI, 2.0f*(float)PI );
				//clamp( ty, -2.0f*(float)PI, 2.0f*(float)PI );
				vertex.mTexCoord[0] = Vector2(tx, ty);
				vertex.mTexCoord[1] = Vector2(tx, ty);

				mVertex.push_back(vertex);

				///////////////////////////////////////////////

				u = cost1[j] * r0;
				v = sint1[j] * r0;
				vertex.mPosition = Vector3(u*radius, v*radius, z0*radius);
				vertex.mNormal = Vector3(u, v, z0);

				tx = u * texWrap; //(atan2(u, z1) / (2.0f * (float)PI) + 0.5f) * texWrap;
				ty = v * texWrap; //(acos(v) / (float)PI + 0.5f) * texWrap;
				//clamp( tx, -2.0f*(float)PI, 2.0f*(float)PI );
				//clamp( ty, -2.0f*(float)PI, 2.0f*(float)PI );
				vertex.mTexCoord[0] = Vector2(tx, ty);
				vertex.mTexCoord[1] = Vector2(tx, ty);

				mVertex.push_back(vertex);
			}

			for (j = vertCount; j < (int)mVertex.size() - 3; j += 2)
			{
				AddIndex(j, j + 1, j + 3, j + 2);
			}
		}

		///* The bottom stack is covered with a triangle fan */

		z0 = z1;
		r0 = r1;

		vertCount = (int)mVertex.size();

		vertex.mPosition = Vector3(0.0f, 0.0f, -radius);
		vertex.mNormal = Vector3(0.0f, 0.0f, -1.0f);

		tx1 = 0.0f; //(atan2(0.0f, -1.0f) / (2.0f * (float)PI) + 0.5f) * texWrap;
		ty1 = 0.0f; //(acos(0.0f) / (float)PI + 0.5f) * texWrap;
		vertex.mTexCoord[0] = Vector2(tx1, ty1);
		vertex.mTexCoord[1] = Vector2(tx1, ty1);

		mVertex.push_back(vertex);

		for (j = 0; j <= slices; j++)
		{
			u = cost1[j] * r0;
			v = sint1[j] * r0;
			vertex.mPosition = Vector3(u*radius, v*radius, z0*radius);
			vertex.mNormal = Vector3(u, v, z0);

			tx1 = u * texWrap; //(atan2(u, z1) / (2.0f * (float)PI) + 0.5f) * texWrap;
			ty1 = v * texWrap; //(acos(v) / (float)PI + 0.5f) * texWrap;
			vertex.mTexCoord[0] = Vector2(tx1, ty1);
			vertex.mTexCoord[1] = Vector2(tx1, ty1);

			mVertex.push_back(vertex);
		}

		for (j = vertCount; j < (int)mVertex.size() - 2; j++)
		{
			AddIndex(vertCount, j + 1, j + 2);
		}

		/* Release sin and cos tables */

		free(sint1);
		free(cost1);
		free(sint2);
		free(cost2);

		mPrimitive = PrimitiveFormat::TRIANGLES;
	}

	// Apply a matrix transform to a set a vertices.
	//
	void MeshBuilder::ApplyMatrix(const Matrix4x4& mat, UINT startVertex, UINT endVertex)
	{
		if (endVertex == UINT_MAX)
			endVertex = mVertex.size();

		for (UINT x = startVertex; x < endVertex; ++x)
		{
			mVertex[x].mPosition = mat.Transform(mVertex[x].mPosition);
		}
	}

	// Create Vertex Buffer.
	//
	void MeshBuilder::CreateVertexBuffer(Renderer* renderer)
	{
		_ASSERT(mVertex.size() > 0);

		auto vertexLayout = mLayout.lock();

		std::vector<VertexAttribute> layout = vertexLayout->Layout();

		UINT vertexSize = vertexLayout->VertexSize();
		UINT numVerts = mVertex.size();
		UINT totalSize = vertexSize * numVerts;

		// Copy the generic vertex into a custom vertex.
		//
		void* memBlock;
		memBlock = malloc(totalSize);
		UCHAR* base = (UCHAR*)memBlock;

		for (UINT j = 0; j < numVerts; ++j)
		{
			UINT texCount = 0;

			for (UINT i = 0; i < (UINT)layout.size(); ++i)
			{
				switch (layout[i])
				{
				case VertexAttribute::POSITION:
					for (UINT k = 0; k < 3; ++k)
					{
						*((float*)base) = mVertex[j].mPosition[k];
						base += sizeof(float);
					}
					break;

				case VertexAttribute::TEXCOORD0:
				case VertexAttribute::TEXCOORD1:
				case VertexAttribute::TEXCOORD2:
					for (UINT k = 0; k < 2; ++k)
					{
						*((float*)base) = mVertex[j].mTexCoord[texCount][k];
						base += sizeof(float);
					}
					++texCount;
					break;

				case VertexAttribute::QUADCOORD0:
				case VertexAttribute::QUADCOORD1:
				case VertexAttribute::QUADCOORD2:
					for (UINT k = 0; k < 4; ++k)
					{
						*((float*)base) = mVertex[j].mQuadCoord[texCount][k];
						base += sizeof(float);
					}
					++texCount;
					break;

				case VertexAttribute::COLOR:
					*((UINT*)base) = mVertex[j].mColor;
					base += sizeof(UINT);
					break;

				case VertexAttribute::NORMAL:
					for (UINT k = 0; k < 3; ++k)
					{
						*((float*)base) = mVertex[j].mNormal[k];
						base += sizeof(float);
					}
					break;

				case VertexAttribute::TANGENT:
					for (UINT k = 0; k < 4; ++k)
					{
						*((float*)base) = mVertex[j].mTangent[k];
						base += sizeof(float);
					}
					break;

				case VertexAttribute::BONE_COUNT:
					*((int*)base) = mVertex[j].mBoneCount;
					base += sizeof(int);
					break;

				case VertexAttribute::BONE_INDEX:
					for (UINT k = 0; k < 4; ++k)
					{
						*((int*)base) = mVertex[j].mBoneIndex[k];
						base += sizeof(int);
					}
					break;

				case VertexAttribute::BONE_WEIGHT:
					for (UINT k = 0; k < 4; ++k)
					{
						*((float*)base) = mVertex[j].mBoneWeight[k];
						base += sizeof(float);
					}
					break;

				case VertexAttribute::MATRIX:
					for (UINT k = 0; k < 16; ++k)
					{
						*((float*)base) = mVertex[j].mMatrix[k];
						base += sizeof(float);
					}
					break;
				}
			}
		}

		mVertexBuffer = renderer->CreateBuffer(memBlock, totalSize, vertexSize, BufferFormat::VERTEX);

		free(memBlock);
	}

	// Create Index Buffer.
	//
	void MeshBuilder::CreateIndexBuffer(Renderer* renderer)
	{
		_ASSERT(mIndex.size() > 0);

		mIndexBuffer = renderer->CreateBuffer(&mIndex.front(), mIndex.size()*sizeof(UINT), sizeof(UINT), BufferFormat::INDEX);
	}

	// Returns the mesh created from the buffers; otherwise, nullptr.
	//
	Mesh* MeshBuilder::BuildMesh(Renderer* renderer, bool createIndexBuffer)
	{
		Mesh* mesh = new Mesh();

		CreateVertexBuffer(renderer);

		if (createIndexBuffer)
			CreateIndexBuffer(renderer);

		mesh->mPrimitive = mPrimitive;
		mesh->mVertexBuffer = mVertexBuffer;
		mesh->mIndexBuffer = mIndexBuffer;
		mesh->mLayout = mLayout;

		mesh->mBounds.Set((BYTE*)mVertex.data(), mVertex.size(), sizeof(Vertex));

		return mesh;
	}

	// Helper functions to build quads for both
	// RenderTextures and GUIs.
	//
	Mesh* MeshBuilder::BuildRenderTextureMesh(Renderer* renderer, std::weak_ptr<VertexLayout>& layout)
	{
		MeshBuilder builder;

		builder.CreateQuad(1.0f);
		builder.mLayout = layout;

		Mesh* mesh = builder.BuildMesh(renderer);

		return mesh;
	}

	Mesh* MeshBuilder::BuildGUIMesh(Renderer* renderer, std::weak_ptr<VertexLayout>& layout)
	{
		MeshBuilder builder;

		builder.CreateQuad(0.5f, Vector3(0, 0, -1));
		builder.mLayout = layout;

		Matrix4x4 matTrans;
		matTrans.Translate(Vector3(0.5f, 0.5f, 0));
		builder.ApplyMatrix(matTrans);

		Mesh* mesh = builder.BuildMesh(renderer);

		return mesh;
	}
}
