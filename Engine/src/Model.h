#pragma once

#include <vector>

#include "Common.h"
#include "Shader.h"
#include "Mesh.h"

namespace Sentinel
{
	class Model;

	extern Model* LoadModelOBJ( const char* filename );
	extern Model* LoadModelM3D( const char* filename );

	class SENTINEL_DLL Model
	{
	public:

		static Shader*  SHADER_COLOR;
		static Shader*  SHADER_TEXTURE;
		static Shader*  SHADER_NORMAL_MAP;
		static Shader*  SHADER_PARALLAX;
		static Shader*  SHADER_SKINNING;

	protected:

		//BoundingSphere	mSphere;

		struct KeyFrame
		{
			Matrix4f	mMatrix;
			int			mFrame;

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

			Matrix4f	mMatrixWorld;
			Matrix4f	mInverseBone;

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

	public:

		Matrix4f		mMatrixWorld;

		/////////////////////////////////////

		virtual ~Model();
		
		static Model*	Load( const char* filename );

		virtual bool	Create( const char* filename ) = 0;
		virtual void	Release() = 0;

		virtual void	SetMaterial( const Material& material ) = 0;
		virtual void	SetShader( Shader* shader ) = 0;

		virtual void	SetKeyFrame( const KeyFrame& key, int keyIndex = -1, int objIndex = 0 ) = 0;
		virtual void	SetTime( float _time, UINT objIndex = 0 ) = 0;
		virtual float	GetTime( UINT objIndex = 0 ) = 0;

		virtual void	Update() = 0;
		virtual void	Draw() = 0;
	};
}
