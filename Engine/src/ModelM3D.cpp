#include <map>
#include <vector>

#include "Model.h"
#include "Mesh.h"
#include "Util.h"
#include "MeshBuilder.h"
#include "Timing.h"
#include "Archive.h"
#include "Renderer.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "Texture.h"
#include "TextureManager.h"
#include "Material.h"

namespace Sentinel
{
	class ModelM3D : public Model
	{
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

		// Materials.
		//
		struct MaterialTexture
		{
			Material	mMaterial;
			std::shared_ptr< Texture > mTexture[ NUM_AUTODESK_TYPES ];
		};

		Object*				mObject;
		UINT				mNumObjects;

		MaterialTexture*	mMaterials;
		UINT				mNumMaterials;

		bool				mIsWeighted;		// Are the vertices weighted?

		int					mVersion;

	public:

		ModelM3D()
		{
			mObject			= NULL;
			mNumObjects		= 0;

			mMaterials		= NULL;
			mNumMaterials	= 0;

			mIsWeighted		= false;

			mMatrixWorld.Identity();
		}

		~ModelM3D()
		{
			Release();
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

		void Save( Archive&			archive,
				   Renderer*		renderer, 
				   ShaderManager*	shaderManager, 
				   TextureManager*	textureManager )
		{
			// Save the format type.
			//
			BYTE format = M3D;
			archive.Write( &format );

			// Save if the model has weighted vertices.
			//
			BYTE weighted = mIsWeighted;
			archive.Write( &weighted );

			// Write the materials.
			//
			archive.Write( &mNumMaterials );
			
			for( UINT x = 0; x < mNumMaterials; ++x )
				WriteMaterial( archive, mMaterials[ x ], textureManager );

			// Write the objects.
			//
			archive.Write( &mNumObjects );

			for( UINT x = 0; x < mNumObjects; ++x )
			{
				// Write the meshes.
				//
				archive.Write( &mObject[ x ].mNumMeshes );

				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
					Mesh::Save( archive, mObject[ x ].mMesh[ y ], renderer, shaderManager, textureManager );

				// Write the keyframes.
				//
				archive.Write( &mObject[ x ].mNumKeyFrames );

				for( UINT y = 0; y < mObject[ x ].mNumKeyFrames; ++y )
				{
					archive.Write( mObject[ x ].mKeyFrame[ y ].mMatrix.Ptr(), 16 );
					archive.Write( &mObject[ x ].mKeyFrame[ y ].mFrame );
				}
				
				// Write the hierarchy.
				//
				if( mObject[ x ].mParent == NULL )
				{
					int hierarchy = -1;
					archive.Write( &hierarchy );
				}
				else
				{
					for( int y = 0; y < (int)mNumObjects; ++y )
					{
						if( mObject[ x ].mParent == &mObject[ y ] )
						{
							archive.Write( &y );
							break;
						}
					}
				}
			}
		}

		void Create( Archive&			archive,
					 Renderer*			renderer, 
					 ShaderManager*		shaderManager, 
					 TextureManager*	textureManager )
		{
			// Read if the model has weighted vertices.
			//
			BYTE weighted;
			archive.Read( &weighted );
			mIsWeighted = (weighted == 1);

			// Read the materials.
			//
			archive.Read( &mNumMaterials );
			mMaterials = new MaterialTexture[ mNumMaterials ];

			for( UINT x = 0; x < mNumMaterials; ++x )
				ReadMaterial( archive, mMaterials[ x ], renderer, textureManager );

			// Read the objects.
			//
			archive.Read( &mNumObjects );
			mObject = new Object[ mNumObjects ];

			for( UINT x = 0; x < mNumObjects; ++x )
			{
				// Read the meshes.
				//
				archive.Read( &mObject[ x ].mNumMeshes );
				mObject[ x ].mMesh = new Mesh*[ mObject[ x ].mNumMeshes ];

				for( UINT y = 0; y < mObject[ x ].mNumMeshes; ++y )
					mObject[ x ].mMesh[ y ] = Mesh::Load( archive, renderer, shaderManager, textureManager );

				// Read the keyframes.
				//
				archive.Read( &mObject[ x ].mNumKeyFrames );
				mObject[ x ].mKeyFrame = new KeyFrame[ mObject[ x ].mNumKeyFrames ];

				for( UINT y = 0; y < mObject[ x ].mNumKeyFrames; ++y )
				{
					archive.Read( mObject[ x ].mKeyFrame[ y ].mMatrix.Ptr(), 16 );
					archive.Read( &mObject[ x ].mKeyFrame[ y ].mFrame );
				}
				
				// Read the hierarchy.
				//
				int hierarchy;
				archive.Read( &hierarchy );

				if( hierarchy == -1 )
					mObject[ x ].mParent = NULL;
				else
					mObject[ x ].mParent = &mObject[ hierarchy ];
			}
		}

		bool Create( const char*		filename, 
					 Renderer*			renderer, 
					 ShaderManager*		shaderManager, 
					 TextureManager*	textureManager )
		{
			Vertex*    vertices  = NULL;
			Vector3f*  normals   = NULL;
			Vector2f*  texCoords = NULL;

			try
			{
				Archive archive;
				if( !archive.Open( filename, "rb" ))
					throw AppException( "Failed to load " + std::string( filename ));

				// Read version.
				//
				archive.Read( &mVersion );

				// Read the materials and create meshes.
				//
				archive.Read( &mNumMaterials );
				mMaterials = new MaterialTexture[ mNumMaterials ];
				for( UINT x = 0; x < mNumMaterials; ++x )
					ReadMaterial( archive, mMaterials[ x ], renderer, textureManager );
				
				// Read whether any data was exported using 32-bit.
				//
				const BYTE WEIGHTED = 0x01;
				const BYTE VERTS_32 = 0x02;
				const BYTE NORMS_32 = 0x04;
				const BYTE TEXCS_32 = 0x08;
				const BYTE INDEX_32 = 0x10;

				BYTE export32;
				archive.Read( &export32 );

				bool bVerts32 = (export32 & VERTS_32) ? true : false;
				bool bNorms32 = (export32 & NORMS_32) ? true : false;
				bool bTexcs32 = (export32 & TEXCS_32) ? true : false;
				bool bIndex32 = (export32 & INDEX_32) ? true : false;

				mIsWeighted = (export32 & WEIGHTED) ? true : false;

				// Read vertices.
				//
				int numVerts;
				archive.Read( &numVerts, 1, bVerts32 );
				vertices = new Vertex[ numVerts ];

				for( int x = 0; x < numVerts; ++x )
				{
					archive.Read( vertices[ x ].mPosition.Ptr(), 3 );

					if( mIsWeighted )
					{
						archive.Read( &vertices[ x ].mNumBones );

						for( int y = 0; y < vertices[ x ].mNumBones; ++y )
						{
							archive.Read( &vertices[ x ].mMatrixIndex[ y ] );
							archive.Read( &vertices[ x ].mWeight[ y ] );
						}
					}
				}

				// Read normals.
				//
				int numNormals;
				archive.Read( &numNormals, 1, bNorms32 );
				normals = new Vector3f[ numNormals ];

				for( int x = 0; x < numNormals; ++x )
					archive.Read( normals[ x ].Ptr(), 3 );

				
				// Read texture coordinates.
				//
				int numTexCoords;
				archive.Read( &numTexCoords, 1, bTexcs32 );
				texCoords = new Vector2f[ numTexCoords ];

				for( int x = 0; x < numTexCoords; ++x )
				{
					archive.Read( texCoords[ x ].Ptr(), 2 );
					texCoords[ x ].y = 1.0f - texCoords[ x ].y;
				}

				// Read fat indices.
				//
				archive.Read( &mNumObjects );
				mObject = new Object[ mNumObjects ];
				int currHierarchy = 0;

				for( UINT x = 0; x < mNumObjects; ++x )
				{
					// Read the hierarchy number.
					//
					int hierarchy;
					archive.Read( &hierarchy );

					if( hierarchy > 0 )
						mObject[ x ].mParent = &mObject[ currHierarchy + hierarchy - 1 ];
					else
						currHierarchy = x;

					// Read if this object is skinned.
					//
					BYTE isSkinned;
					archive.Read( &isSkinned );

					// Read the keyframe animations.
					//
					archive.Read( &mObject[ x ].mNumKeyFrames );
					mObject[ x ].mKeyFrame = new KeyFrame[ mObject[ x ].mNumKeyFrames ];

					for( UINT y = 0; y < mObject[ x ].mNumKeyFrames; ++y )
					{
						KeyFrame* currKey = &mObject[ x ].mKeyFrame[ y ];

						// Read matrix.
						//
						archive.Read( currKey->mMatrix.Ptr(), 16 );
						
						// Read frame timestamp.
						//
						archive.Read( &currKey->mFrame );
					}

					// Prepare keyframes for skinned animation if necessary.
					//
					if( mIsWeighted )
						mObject[ x ].mInverseBone = mObject[ x ].mKeyFrame[ 0 ].mMatrix.Inverse();
					
					// Read indices.
					//
					archive.Read( &mObject[ x ].mNumMeshes );
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
						archive.Read( &matID );

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
							builder.mShader = shaderManager->Get( "Color" );
						}
						else
						if( isSkinned )
						{
							builder.mShader = shaderManager->Get( "Skinning" );
						}
						else
						if( numTextures == 1 )
						{
							builder.mShader = shaderManager->Get( "Texture" );
						}
						else
						if( numTextures == 2 )
						{
							builder.mShader = shaderManager->Get( "Normal Map" );
						}
						else
						{
							builder.mShader = shaderManager->Get( "Parallax" );
						}

						// Read faces.
						//
						MeshBuilder::Vertex meshVertex;

						int count;
						archive.Read( &count, 1, bIndex32 );

						for( int z = 0; z < count; ++z )
						{
							int vertex;
							archive.Read( &vertex, 1, bVerts32 );

							int normal;
							archive.Read( &normal, 1, bNorms32 );

							int texCoord;
							archive.Read( &texCoord, 1, bTexcs32 );

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
						
						mObject[ x ].mMesh[ y ] = builder.BuildMesh( renderer );
						mObject[ x ].mMesh[ y ]->mMaterial = material;

						builder.ApplyMatrix( mObject[ x ].mKeyFrame[ 0 ].mMatrix );

						builder.ClearAll();
					}
				}

				delete[] vertices;
				delete[] normals;
				delete[] texCoords;

				archive.Close();
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

	private:

		void WriteMaterial( Archive& archive, MaterialTexture& matTex, TextureManager* textureManager )
		{
			// Write base material.
			//
			archive.Write( matTex.mMaterial.mAmbient.Ptr(),  3 );
			archive.Write( matTex.mMaterial.mDiffuse.Ptr(),  3 );
			archive.Write( matTex.mMaterial.mSpecular.Ptr(), 3 );

			float spec_comp = matTex.mMaterial.mSpecularComponent / 100.0f;
			archive.Write( &spec_comp );

			float alpha = matTex.mMaterial.mAmbient.a + matTex.mMaterial.mDiffuse.a + matTex.mMaterial.mSpecular.a;
			archive.Write( &alpha );

			// Write filenames of each texture.
			//
			UINT numTextures = 0;
			for( UINT x = 0; x < NUM_AUTODESK_TYPES; ++x )
				if( matTex.mTexture[ x ] != NULL )
					++numTextures;

			archive.Write( &numTextures, 1, false );

			for( BYTE x = 0; x < NUM_AUTODESK_TYPES; ++x )
			{
				if( matTex.mTexture[ x ] != NULL )
				{
					archive.Write( &x );

					std::string name = textureManager->Get( matTex.mTexture[ x ] );

					archive.Write( &name );
				}
			}
		}

		void ReadMaterial( Archive& archive, MaterialTexture& matTex, Renderer* renderer, TextureManager* textureManager )
		{
			// Set the material.
			//
			Vector3f ambient;
			archive.Read( ambient.Ptr(), ar_sizeof( ambient ));
			
			Vector3f diffuse;
			archive.Read( diffuse.Ptr(), ar_sizeof( diffuse ));
			
			Vector3f specular;
			archive.Read( specular.Ptr(), ar_sizeof( specular ));
			
			float spec_comp;
			archive.Read( &spec_comp );
			spec_comp *= 100.0f;

			float alpha;
			archive.Read( &alpha );
			
			matTex.mMaterial = Material( ColorRGBA( ambient.x,  ambient.y,  ambient.z,  alpha ),\
										 ColorRGBA( diffuse.x,  diffuse.y,  diffuse.z,  0 ),\
										 ColorRGBA( specular.x, specular.y, specular.z, 0 ),\
										 spec_comp );

			// Read filenames of each texture.
			//
			UINT numTextures;
			archive.Read( &numTextures, 1, false );

			for( UINT x = 0; x < numTextures; ++x )
			{
				BYTE type;
				archive.Read( &type );

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

				std::string name;
				archive.Read( &name );
				
				if( textureManager )
					matTex.mTexture[ type ] = textureManager->Add( name, renderer->CreateTextureFromFile( name.c_str() ));
				else
					matTex.mTexture[ type ] = renderer->CreateTextureFromFile( name.c_str() );
			}
		}

	public:

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
		
		void Update( float DT )
		{
			// Setup Bone Matrix.
			//
			/*
			if( mIsWeighted )
			{
				static Matrix4f matBone;

				std::shared_ptr< Shader > skin = ShaderManager::Inst()->Get( "Skinning" );

				Renderer::Inst()->SetShader( skin );

				for( UINT x = 0; x < mNumObjects; ++x )
				{
					matBone = mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mMatrix * mObject[ x ].mInverseBone;

					// Set the B designation first to ensure this works correctly.
					//
					skin->SetMatrix( skin->Uniform().find( 'B' ), matBone.Ptr(), x, 1 );
				}
			}
			*/

			for( UINT x = 0; x < mNumObjects; ++x )
			{
				// Adjust keyframe for animation times.
				//
				if( mObject[ x ].mNumKeyFrames > 0 )
				{
					mObject[ x ].mCurrTime += DT * 30.0f;

					if( mObject[ x ].mKeyFrame[ mObject[ x ].mCurrKey ].mFrame <= (int)mObject[ x ].mCurrTime )
					{
						++mObject[ x ].mCurrKey;

						if( (int)mObject[ x ].mCurrKey >= (int)mObject[ x ].mNumKeyFrames )
						{
							mObject[ x ].mCurrTime = 0;
							mObject[ x ].mCurrKey  = 0;
						}
					}
				}
			}
		}

		void Draw( Renderer* renderer, GameWorld* world )
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
					mObject[ x ].mMesh[ y ]->Draw( renderer, world );
				}
			}
		}
	};

	// Create an M3D Model.
	//
	Model* LoadModelM3DFromFile( const char*		filename, 
								 Renderer*			renderer, 
								 ShaderManager*		shaderManager, 
								 TextureManager*	textureManager )
	{
		ModelM3D* model = new ModelM3D();

		model->Create( filename, renderer, shaderManager, textureManager );

		return model;
	}

	Model* LoadModelM3DFromArchive( Archive&			archive,
									Renderer*			renderer, 
									ShaderManager*		shaderManager, 
									TextureManager*		textureManager )
	{
		ModelM3D* model = new ModelM3D();

		model->Create( archive, renderer, shaderManager, textureManager );

		return model;
	}
}
