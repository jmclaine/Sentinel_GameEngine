#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include "Model.h"
#include "Util.h"
#include "MeshBuilder.h"
#include "Timing.h"

namespace Sentinel
{
	bool gReadASCII = false;

	#define READ_FROM_FILE( a, b ) { (gReadASCII) ? a : b; }

	#define OPEN_FILE( file_ptr, filename, mode, mode_bin )\
		(gReadASCII) ? file_ptr.open( filename, mode ) : \
					   file_ptr.open( filename, mode_bin )

	//////////////////////////////////////////

	class ModelM3D : public Model
	{
		Object*		mObject;
		UINT		mNumObjects;

		struct Vertex
		{
			Vector3f	mPosition;

			// Bones.
			//
			float	mWeight[ 4 ];
			int		mMatrixIndex[ 4 ];
			int		mNumBones;

			Vertex() : mNumBones( 0 ) {}
		};
		bool isWeighted;		// Are the vertices weighted?

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
		MaterialTexture*	mMaterials;
		UINT				mNumMaterials;

	private:

		void ReadMaterial( std::ifstream& file, MaterialTexture& matTex )
		{
			// Set the material.
			//
			Vector3f color;
			color = ReadPoint3( file );
			ColorRGBA ambient( color.x, color.y, color.z );

			color = ReadPoint3( file );
			ColorRGBA diffuse( color.x, color.y, color.z );

			color = ReadPoint3( file );
			ColorRGBA specular( color.x, color.y, color.z );

			float spec_comp = ReadFloat( file ) * 100.0f;

			matTex.mMaterial = Material( ambient, diffuse, specular, spec_comp );

			// Read filenames of each texture.
			//
			int numTextures = ReadInt( file );

			char name[ 256 ];
			for( int x = 0; x < numTextures; ++x )
			{
				int type = ReadInt( file );

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

				int len = ReadInt( file );
				ReadString( file, name, len );

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

			isWeighted		= false;

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
				std::ifstream file;

				OPEN_FILE( file, filename, std::ios::in, std::ios::binary );
				if( !file.is_open() )
					throw AppException( "Failed to load " + std::string( filename ));

				// Read version.
				//
				ReadInt( file );

				// Read the materials and create meshes.
				//
				mNumMaterials = ReadInt( file );
				mMaterials = new MaterialTexture[ mNumMaterials ];
				for( UINT x = 0; x < mNumMaterials; ++x )
				{
					ReadMaterial( file, mMaterials[ x ] );
				}

				// Read the smallest bounding sphere.
				//
				Vector3f center = ReadPoint3( file );
				float radius = ReadFloat( file );

				// Read whether any data should be exported using 32-bit.
				//
				const BYTE WEIGHTED = 0x01;
				const BYTE VERTS_32 = 0x02;
				const BYTE NORMS_32 = 0x04;
				const BYTE TEXCS_32 = 0x08;
				const BYTE INDEX_32 = 0x10;

				BYTE export32 = ReadByte( file );

				bool bVerts32 = (export32 & VERTS_32) ? true : false;
				bool bNorms32 = (export32 & NORMS_32) ? true : false;
				bool bTexcs32 = (export32 & TEXCS_32) ? true : false;
				bool bIndex32 = (export32 & INDEX_32) ? true : false;

				isWeighted = (export32 & WEIGHTED) ? true : false;

				// Read vertices.
				//
				int numVerts = ReadInt( file, bVerts32 );
				vertices = new Vertex[ numVerts ];

				for( int x = 0; x < numVerts; ++x )
				{
					vertices[ x ].mPosition = ReadPoint3( file );

					if( isWeighted )
					{
						vertices[ x ].mNumBones = ReadInt( file );

						for( int y = 0; y < vertices[ x ].mNumBones; ++y )
						{
							vertices[ x ].mMatrixIndex[ y ] = ReadInt( file );
							vertices[ x ].mWeight[ y ]		= ReadFloat( file );
						}
					}
				}

				// Read normals.
				//
				int numNormals = ReadInt( file, bNorms32 );
				normals = new Vector3f[ numNormals ];

				for( int x = 0; x < numNormals; ++x )
				{
					normals[ x ] = ReadPoint3( file );
				}

				// Read texture coordinates.
				//
				int numTexCoords = ReadInt( file, bTexcs32 );
				texCoords = new Vector2f[ numTexCoords ];

				for( int x = 0; x < numTexCoords; ++x )
				{
					texCoords[ x ] = ReadPoint2( file );
					texCoords[ x ].y = 1.0f - texCoords[ x ].y;
				}

				// Read fat indices.
				//
				mNumObjects = ReadInt( file );
				mObject = new Object[ mNumObjects ];
				int currHierarchy = 0;

				for( UINT x = 0; x < mNumObjects; ++x )
				{
					// Read the hierarchy number.
					//
					int hierarchy = ReadInt( file );
					if( hierarchy > 0 )
						mObject[ x ].mParent = &mObject[ currHierarchy + hierarchy - 1 ];
					else
						currHierarchy = x;

					// Read if this object is skinned.
					//
					bool isSkinned = ReadByte( file ) > 0;

					// Read the keyframe animations.
					//
					mObject[ x ].mNumKeyFrames = ReadInt( file );
					mObject[ x ].mKeyFrame = new KeyFrame[ mObject[ x ].mNumKeyFrames ];

					for( UINT y = 0; y < mObject[ x ].mNumKeyFrames; ++y )
					{
						KeyFrame* currKey = &mObject[ x ].mKeyFrame[ y ];

						// Read matrix.
						//
						for( int z = 0; z < 4; ++z )
						{
							for( int w = 0; w < 4; ++w )
							{
								float f = ReadFloat( file );
								currKey->mMatrix[ (z<<2)+w ] = f;
							}
						}

						// Read frame timestamp.
						//
						currKey->mFrame = ReadInt( file );
					}

					// Prepare keyframes for skinned animation if necessary.
					//
					if( isWeighted )
						mObject[ x ].mInverseBone = mObject[ x ].mKeyFrame[ 0 ].mMatrix.Inverse();
					
					// Read indices.
					//
					mObject[ x ].mNumMeshes = ReadInt( file );
					mObject[ x ].mMesh = new Mesh*[ mObject[ x ].mNumMeshes ];

					for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
						mObject[ x ].mMesh[ y ] = NULL;

					MeshBuilder builder;

					for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
					{
						builder.ClearAll();

						// Count the number of textures used.
						//
						UINT numTextures = 0;

						// Read the material ID for this object.
						//
						int matID = ReadInt( file );
						if( matID != 65535 )
						{
							MaterialTexture& mtex = mMaterials[ matID ];
							builder.mMaterial = mtex.mMaterial;

							for( UINT z = 0; z < NUM_AUTODESK_TYPES; ++z )
							{
								if( mtex.mTexture[ z ] != NULL )
								{
									if( builder.mTexture[ numTextures ] = mtex.mTexture[ z ] )
									{
										++numTextures;
									}
								}
							}
						}

						if( numTextures == 0 )
						{
							if( SHADER_COLOR == NULL )
								throw AppException( "ModelM3D::SHADER_COLOR not set!" );

							builder.mShader = SHADER_COLOR;
						}
						else
						if( isSkinned )
						{
							if( SHADER_SKINNING == NULL )
								throw AppException( "ModelM3D::SHADER_SKINNING not set!" );

							builder.mShader = SHADER_SKINNING;
						}
						else
						if( numTextures == 1 )
						{
							if( SHADER_TEXTURE == NULL )
								throw AppException( "ModelM3D::SHADER_TEXTURE not set!" );

							builder.mShader = SHADER_TEXTURE;
						}
						else
						if( numTextures == 2 )
						{
							if( SHADER_NORMAL_MAP == NULL )
								throw AppException( "ModelM3D::SHADER_NORMAL_MAP not set!" );

							builder.mShader = SHADER_NORMAL_MAP;
						}
						else
						{
							if( SHADER_PARALLAX == NULL )
								throw AppException( "ModelM3D::SHADER_PARALLAX not set!" );

							builder.mShader = SHADER_PARALLAX;
						}

						// Read faces.
						//
						MeshBuilder::Vertex meshVertex;

						int count = ReadInt( file, bIndex32 );
						for( int z = 0; z < count; ++z )
						{
							int vertex   = ReadInt( file, bVerts32 );
							int normal   = ReadInt( file, bNorms32 );
							int texCoord = ReadInt( file, bTexcs32 );

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

						builder.ApplyMatrix( mObject[ x ].mKeyFrame[ 0 ].mMatrix );

						/*BoundingSphere sphere = FindSmallestSphere( &builder.vertices().front(), builder.numVertices() );
						if( sphere.radius > m_sphere.radius )
						{
							m_sphere = sphere;
						}*/

						builder.ClearAll();
					}
				}

				delete[] vertices;
				delete[] normals;
				delete[] texCoords;

				file.close();
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

		void SetMaterial( const Material& material )
		{
			for( UINT x = 0; x < mNumObjects; ++x )
			{
				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
				{
					mObject[ x ].mMesh[ y ]->SetMaterial( material );
				}
			}
		}

		void SetShader( Shader* shader )
		{
			for( UINT x = 0; x < mNumObjects; ++x )
			{
				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
				{
					mObject[ x ].mMesh[ y ]->SetShader( shader );
				}
			}
		}

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
			if( isWeighted )
			{
				static Matrix4f matBone;

				Renderer::Inst()->SetShader( SHADER_SKINNING );

				for( UINT x = 0; x < mNumObjects; ++x )
				{
					_ASSERT(0);

					matBone = mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mMatrix * mObject[ x ].mInverseBone;
					//m_shaderSkinning->setMatrix( SHADER_UNIF_BONE_MATRIX, matBone.m, x, 1 );
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
				{
					matWorldObject = mMatrixWorld * mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mMatrix;
				}
				else
				{
					matWorldObject = mObject[ x ].mParent->mMatrixWorld * mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mMatrix;
				}

				mObject[ x ].mMatrixWorld = matWorldObject;

				// Render each mesh by material.
				//
				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
				{
					mObject[ x ].mMesh[ y ]->SetWorldTransform( matWorldObject );
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
