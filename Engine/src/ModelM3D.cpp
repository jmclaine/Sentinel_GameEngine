#include <map>
#include <vector>

#include "FileIO.h"
#include "Model.h"
#include "Util.h"
#include "MeshBuilder.h"
#include "Timing.h"

namespace Sentinel
{
	class ModelM3D : public Model
	{
		struct Vertex
		{
			Vector3f	mPosition;

			// Bones.
			//
			float		mWeight[ 4 ];
			int			mMatrixIndex[ 4 ];
			int			mNumBones;

			Vertex() : mNumBones( 0 ) {}
		};
		
		// Texture types from 3DStudio Max 2012.
		//
		enum AutodeskTextureTypes
		{
			AUTODESK_DIFFUSE1 = 1,
			AUTODESK_DIFFUSE2 = 2,
			AUTODESK_DIFFUSE3 = 3,
			AUTODESK_DIFFUSE4 = 4,
			AUTODESK_DIFFUSE5 = 5,
			AUTODESK_DIFFUSE6 = 6,
			AUTODESK_DIFFUSE7 = 7,
			AUTODESK_BUMP     = 8,
			AUTODESK_PARALLAX = 9,	// Refraction

			NUM_AUTODESK_TYPES,
		};

		// Materials.
		//
		struct MaterialTexture
		{
			Material	mMaterial;
			Texture*	mTexture[ NUM_AUTODESK_TYPES ];

			MaterialTexture()
			{
				for( UINT x = 0; x < NUM_AUTODESK_TYPES; ++x )
					mTexture[ x ] = NULL;
			}

			~MaterialTexture()
			{
				for( UINT x = 0; x < NUM_AUTODESK_TYPES; ++x )
					SAFE_DELETE( mTexture[ x ] );
			}
		};

		Object*				mObject;
		UINT				mNumObjects;

		MaterialTexture*	mMaterials;
		UINT				mNumMaterials;

		bool				mIsWeighted;		// Are the vertices weighted?

	private:

		void ReadMaterial( FILE* file, MaterialTexture& matTex )
		{
			// Set the material.
			//
			Vector3f color;
			FileIO::Read( file, color.Ptr(), 3 );
			ColorRGBA ambient( color.x, color.y, color.z );

			FileIO::Read( file, color.Ptr(), 3 );
			ColorRGBA diffuse( color.x, color.y, color.z );

			FileIO::Read( file, color.Ptr(), 3 );
			ColorRGBA specular( color.x, color.y, color.z );

			float spec_comp;
			FileIO::Read( file, spec_comp );
			spec_comp *= 100.0f;

			matTex.mMaterial = Material( ambient, diffuse, specular, spec_comp );

			// Read filenames of each texture.
			//
			int numTextures;
			FileIO::Read( file, numTextures );

			char name[ 256 ];
			for( int x = 0; x < numTextures; ++x )
			{
				int type;
				FileIO::Read( file, type );

				if( type == AUTODESK_BUMP )
				{
					type = TEXTURE_NORMAL;
				}
				else
				if( type == AUTODESK_PARALLAX )
				{
					type = TEXTURE_DEPTH;
				}
				else
				{
					type = TEXTURE_DIFFUSE;
				}

				int len;
				FileIO::Read( file, len );
				FileIO::Read( file, name, len );

				matTex.mTexture[ type ] = Renderer::Inst()->CreateTextureFromFile( name );
			}
		}

	public:

		ModelM3D( const char* filename )
		{
			mObject			= NULL;
			mNumObjects		= 0;

			mMaterials		= NULL;
			mNumMaterials	= 0;

			mIsWeighted		= false;

			mMatrixWorld.Identity();

			Create( filename );
		}

		~ModelM3D()
		{
			Release();
		}

		bool Create( const char* filename )
		{
			Vertex*    vertices  = NULL;
			Vector3f*  normals   = NULL;
			Vector2f*  texCoords = NULL;

			try
			{
				FILE* file = fopen( filename, "rb" );

				if( !file )
					throw AppException( "Failed to load " + std::string( filename ));

				// Read version.
				//
				int version;
				FileIO::Read( file, version );

				// Read the materials and create meshes.
				//
				FileIO::Read( file, mNumMaterials );
				mMaterials = new MaterialTexture[ mNumMaterials ];
				for( UINT x = 0; x < mNumMaterials; ++x )
					ReadMaterial( file, mMaterials[ x ] );
				
				// Read whether any data was exported using 32-bit.
				//
				const BYTE WEIGHTED = 0x01;
				const BYTE VERTS_32 = 0x02;
				const BYTE NORMS_32 = 0x04;
				const BYTE TEXCS_32 = 0x08;
				const BYTE INDEX_32 = 0x10;

				BYTE export32;
				FileIO::Read( file, export32 );

				bool bVerts32 = (export32 & VERTS_32) ? true : false;
				bool bNorms32 = (export32 & NORMS_32) ? true : false;
				bool bTexcs32 = (export32 & TEXCS_32) ? true : false;
				bool bIndex32 = (export32 & INDEX_32) ? true : false;

				mIsWeighted = (export32 & WEIGHTED) ? true : false;

				// Read vertices.
				//
				int numVerts;
				FileIO::Read( file, numVerts, bVerts32 );
				vertices = new Vertex[ numVerts ];

				for( int x = 0; x < numVerts; ++x )
				{
					FileIO::Read( file, vertices[ x ].mPosition.Ptr(), 3 );

					if( mIsWeighted )
					{
						FileIO::Read( file, vertices[ x ].mNumBones );

						for( int y = 0; y < vertices[ x ].mNumBones; ++y )
						{
							FileIO::Read( file, vertices[ x ].mMatrixIndex[ y ] );
							FileIO::Read( file, vertices[ x ].mWeight[ y ] );
						}
					}
				}

				// Read normals.
				//
				int numNormals;
				FileIO::Read( file, numNormals, bNorms32 );
				normals = new Vector3f[ numNormals ];

				for( int x = 0; x < numNormals; ++x )
					FileIO::Read( file, normals[ x ].Ptr(), 3 );

				
				// Read texture coordinates.
				//
				int numTexCoords;
				FileIO::Read( file, numTexCoords, bTexcs32 );
				texCoords = new Vector2f[ numTexCoords ];

				for( int x = 0; x < numTexCoords; ++x )
				{
					FileIO::Read( file, texCoords[ x ].Ptr(), 2 );
					texCoords[ x ].y = 1.0f - texCoords[ x ].y;
				}

				// Read fat indices.
				//
				FileIO::Read( file, mNumObjects );
				mObject = new Object[ mNumObjects ];
				int currHierarchy = 0;

				for( UINT x = 0; x < mNumObjects; ++x )
				{
					// Read the hierarchy number.
					//
					int hierarchy;
					FileIO::Read( file, hierarchy );

					if( hierarchy > 0 )
						mObject[ x ].mParent = &mObject[ currHierarchy + hierarchy - 1 ];
					else
						currHierarchy = x;

					// Read if this object is skinned.
					//
					BYTE isSkinned;
					FileIO::Read( file, isSkinned );

					// Read the keyframe animations.
					//
					FileIO::Read( file, mObject[ x ].mNumKeyFrames );
					mObject[ x ].mKeyFrame = new KeyFrame[ mObject[ x ].mNumKeyFrames ];

					for( UINT y = 0; y < mObject[ x ].mNumKeyFrames; ++y )
					{
						KeyFrame* currKey = &mObject[ x ].mKeyFrame[ y ];

						// Read matrix.
						//
						//FileIO::Read( file, currKey->mMatrix );
						FileIO::Read( file, currKey->mMatrix.Ptr(), 16 );
						
						// Read frame timestamp.
						//
						FileIO::Read( file, currKey->mFrame );
					}

					// Prepare keyframes for skinned animation if necessary.
					//
					if( mIsWeighted )
						mObject[ x ].mInverseBone = mObject[ x ].mKeyFrame[ 0 ].mMatrix.Inverse();
					
					// Read indices.
					//
					FileIO::Read( file, mObject[ x ].mNumMeshes );
					mObject[ x ].mMesh = new Mesh*[ mObject[ x ].mNumMeshes ];

					for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
						mObject[ x ].mMesh[ y ] = NULL;

					MeshBuilder builder;
					Material	material;

					for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
					{
						builder.ClearAll();

						// Count the number of textures used.
						//
						UINT numTextures = 0;

						// Read the material ID for this object.
						//
						int matID;
						FileIO::Read( file, matID );

						if( matID != -1 )
						{
							MaterialTexture& mtex = mMaterials[ matID ];
							material = mtex.mMaterial;

							for( UINT z = 0; z < NUM_AUTODESK_TYPES; ++z )
								if( mtex.mTexture[ z ] != NULL )
									if( builder.mTexture[ numTextures ] = mtex.mTexture[ z ] )
										++numTextures;
						}
						else
						{
							material = Material();
						}

						if( numTextures == 0 )
						{
							_ASSERT( SHADER_COLOR );

							builder.mShader = SHADER_COLOR;
						}
						else
						if( isSkinned )
						{
							_ASSERT( SHADER_SKINNING );

							builder.mShader = SHADER_SKINNING;
						}
						else
						if( numTextures == 1 )
						{
							_ASSERT( SHADER_TEXTURE );
							
							builder.mShader = SHADER_TEXTURE;
						}
						else
						if( numTextures == 2 )
						{
							_ASSERT( SHADER_NORMAL_MAP );
							
							builder.mShader = SHADER_NORMAL_MAP;
						}
						else
						{
							_ASSERT( SHADER_PARALLAX );
							
							builder.mShader = SHADER_PARALLAX;
						}

						// Read faces.
						//
						MeshBuilder::Vertex meshVertex;

						int count;
						FileIO::Read( file, count, bIndex32 );

						for( int z = 0; z < count; ++z )
						{
							int vertex;
							FileIO::Read( file, vertex, bVerts32 );

							int normal;
							FileIO::Read( file, normal, bNorms32 );

							int texCoord;
							FileIO::Read( file, texCoord, bTexcs32 );

							meshVertex.mPosition = vertices[ vertex ].mPosition;

							if( isSkinned )
							{
								COPY_ARRAY( meshVertex.mWeight, vertices[ vertex ].mWeight, 4 );
								COPY_ARRAY( meshVertex.mMatrixIndex, vertices[ vertex ].mMatrixIndex, 4 );
								meshVertex.mNumBones = vertices[ vertex ].mNumBones;
							}

							meshVertex.mNormal = normals[ normal ];
							meshVertex.mTextureCoords[ 0 ] = Vector2f( texCoords[ texCoord ].x, texCoords[ texCoord ].y );

							builder.mVertex.push_back( meshVertex );

							builder.AddIndex( z );
						}

						builder.CalculateTangents( false );
						
						mObject[ x ].mMesh[ y ] = builder.BuildMesh();
						mObject[ x ].mMesh[ y ]->mMaterial = material;

						builder.ApplyMatrix( mObject[ x ].mKeyFrame[ 0 ].mMatrix );

						builder.ClearAll();
					}
				}

				delete[] vertices;
				delete[] normals;
				delete[] texCoords;

				fclose( file );
			}
			catch( AppException e )
			{
				REPORT_ERROR( e.what(), "Model Load Failure" );

				SAFE_DELETE_ARRAY( vertices );
				SAFE_DELETE_ARRAY( normals );
				SAFE_DELETE_ARRAY( texCoords );
				
				Release();
				
				return false;
			}

			return true;
		}

		void Release()
		{
			for( UINT x = 0; x < mNumObjects; ++x )
			{
				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
					SAFE_DELETE( mObject[ x ].mMesh[ y ] );
				
				SAFE_DELETE_ARRAY( mObject[ x ].mMesh );
				SAFE_DELETE_ARRAY( mObject[ x ].mKeyFrame );
			}

			SAFE_DELETE_ARRAY( mMaterials );
			SAFE_DELETE_ARRAY( mObject );

			mNumObjects   = 0;
			mNumMaterials = 0;
		}

		//////////////////////////////////////////////////////////////////////////

		void SetMaterials( const std::vector< Material >& material )
		{
			auto it = material.begin();

			for( UINT x = 0; x < mNumObjects; ++x )
				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
				{
					mObject[ x ].mMesh[ y ]->mMaterial = *it;
					++it;
				}
		}

		void GetMaterials( std::vector< Material >* material )
		{
			material->clear();

			for( UINT x = 0; x < mNumObjects; ++x )
				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
					material->push_back( mObject[ x ].mMesh[ y ]->mMaterial );
		}
		/*
		void SetShader( Shader* shader )
		{
			for( UINT x = 0; x < mNumObjects; ++x )
				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
					mObject[ x ].mMesh[ y ]->mShader = shader;
		}
		*/
		// Set a keyframe, and append it if the index is -1.
		//
		void SetKeyFrame( const KeyFrame& key, int keyIndex = -1, int objIndex = 0 )
		{
			_ASSERT( objIndex >= 0 && (UINT)objIndex < mNumObjects && keyIndex > -1 && keyIndex < (int)mObject[ objIndex ].mNumKeyFrames );

			mObject[ objIndex ].mKeyFrame[ keyIndex ] = key;
		}

		void SetTime( float _time, UINT objIndex = 0 )
		{
			_ASSERT( objIndex < mNumObjects );

			if( (UINT)_time < mObject[ objIndex ].mNumKeyFrames )
			{
				mObject[ objIndex ].mCurrTime = _time;
				mObject[ objIndex ].mCurrKey  = (int)_time;
			}
			else
			{
				mObject[ objIndex ].mCurrTime = 0;
				mObject[ objIndex ].mCurrKey  = 0;
			}
		}

		float GetTime( UINT objIndex = 0 )
		{
			_ASSERT( objIndex < mNumObjects );

			return mObject[ objIndex ].mCurrTime;
		}
		
		void Update()
		{
			// Setup Bone Matrix.
			//
			if( mIsWeighted )
			{
				static Matrix4f matBone;

				Renderer::Inst()->SetShader( SHADER_SKINNING );

				for( UINT x = 0; x < mNumObjects; ++x )
				{
					matBone = mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mMatrix * mObject[ x ].mInverseBone;

					// Set the B designation first to ensure this works correctly.
					//
					SHADER_SKINNING->SetMatrix( SHADER_SKINNING->UniformDecl().find( 'B' ), matBone.Ptr(), x, 1 );
				}
			}

			for( UINT x = 0; x < mNumObjects; ++x )
			{
				// Adjust keyframe for animation times.
				//
				if( mObject[ x ].mNumKeyFrames > 0 )
				{
					mObject[ x ].mCurrTime += Timing::Inst()->DeltaTime() * 30.0f;

					if( mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mFrame <= (int)mObject[ x ].mCurrTime )
					{
						++mObject[ x ].mCurrKey;

						if( (int)mObject[ x ].mCurrKey >= (int)mObject[ x ].mNumKeyFrames )
						{
							mObject[ x ].mCurrTime = 0;
							mObject[ x ].mCurrKey = 0;
						}
					}
				}
			}
		}

		void Draw()
		{
			// Set up model transforms.
			//
			for( UINT x = 0; x < mNumObjects; ++x )
			{
				static Matrix4f matWorldObject;

				if( mObject[ x ].mParent == NULL )
					matWorldObject = mMatrixWorld * mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mMatrix;
				else
					matWorldObject = mObject[ x ].mParent->mMatrixWorld * mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mMatrix;
				
				mObject[ x ].mMatrixWorld = matWorldObject;

				// Render each mesh by material.
				//
				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
				{
					mObject[ x ].mMesh[ y ]->mMatrixWorld = matWorldObject;
					mObject[ x ].mMesh[ y ]->Draw();
				}
			}
		}
	};

	// Create an M3D Model Loader.
	//
	Model* LoadModelM3D( const char* filename )
	{
		return new ModelM3D( filename );
	}
}
