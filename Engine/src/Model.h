#pragma once

#include <vector>

#include "MathLib.h"
#include "Timing.h"
#include "Renderer.h"
#include "Mesh.h"

namespace Sentinel
{
	class Model;

	extern Model* LoadModelOBJ( const char* filename );
	extern Model* LoadModelM3D( const char* filename );

	class Model
	{
	public:

		static Shader*  SHADER_COLOR;
		static Shader*  SHADER_TEXTURE;
		static Shader*  SHADER_NORMAL_MAP;
		static Shader*  SHADER_PARALLAX;
		static Shader*  SHADER_SKINNING;

		mat4f			mMatrixWorld;

		//BoundingSphere	m_sphere;

		struct KeyFrame
		{
			mat4f	mMatrix;
			int		mFrame;

			KeyFrame()
			{
				mMatrix.Identity();
				mFrame = 0;
			}
		};

		struct Object
		{
			Mesh**		mMesh;
			UINT		mNumMeshes;

			KeyFrame*	mKeyFrame;
			UINT		mNumKeyFrames;

			float		mCurrTime;
			int			mCurrKey;

			mat4f		mMatrixWorld;
			mat4f		mInverseBone;

			Object*		mParent;

			Object()
			{
				mParent = NULL;

				mCurrTime = 0.0f;
				mCurrKey = 0;

				mMatrixWorld.Identity();
				mInverseBone.Identity();
			}

			~Object()
			{
				SAFE_DELETE_ARRAY( mMesh );
				SAFE_DELETE_ARRAY( mKeyFrame );
			}
		};

		virtual ~Model() {}
		
		static Model* Load( const char* filename )
		{
			// Determine the extension of the object,
			// and load it according to its type.
			//
			int len = strlen( filename ) - 1;
			if( toupper(filename[ len - 2 ]) == 'O' && toupper(filename[ len - 1 ]) == 'B' && toupper(filename[ len ]) == 'J' )
			{
				//return LoadModelOBJ( filename );
			}
			else
			if( toupper(filename[ len - 2 ]) == 'M' && toupper(filename[ len - 1 ]) == '3' && toupper(filename[ len ]) == 'D' )
			{
				return LoadModelM3D( filename );
			}

			return NULL;
		}

		virtual void SetMaterial( const Material& material ) = 0;
		virtual void SetShader( Shader* shader ) = 0;

		virtual void  SetKeyFrame( const KeyFrame& key, int keyIndex = -1, int objIndex = 0 ) = 0;
		virtual void  SetTime( float _time, UINT objIndex = 0 ) = 0;
		virtual float GetTime( UINT objIndex = 0 ) = 0;

		virtual void Release() = 0;
		virtual bool Create( const char* filename ) = 0;

		virtual void Update() = 0;
		virtual void Draw() = 0;
	};
}
