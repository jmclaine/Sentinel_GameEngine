#include <map>
#include <vector>

#include "Debug.h"
#include "Model.h"
#include "Mesh.h"
#include "Util.h"
#include "MeshBuilder.h"
#include "Timing.h"
#include "Archive.h"
#include "Renderer.h"
#include "Component/Camera.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Material.h"

namespace Sentinel
{
	class ModelM3D : public Model
	{
		// Texture types from 3DStudio Max 2012.
		//
		enum AutodeskTextureType
		{
			AUTODESK_DIFFUSE1 = 1,
			AUTODESK_DIFFUSE2 = 2,
			AUTODESK_DIFFUSE3 = 3,
			AUTODESK_DIFFUSE4 = 4,
			AUTODESK_DIFFUSE5 = 5,
			AUTODESK_DIFFUSE6 = 6,
			AUTODESK_DIFFUSE7 = 7,
			AUTODESK_BUMP = 8,
			AUTODESK_PARALLAX = 9,	// Refraction

			NUM_AUTODESK_TYPES,
		};

		struct Vertex
		{
			Vector3 mPosition;

			// Bones.
			//
			int mBoneCount;
			int mBoneIndex[4];
			float mBoneWeight[4];

			Vertex() : mBoneCount(0) {}
		};

		struct KeyFrame
		{
			Matrix4x4 mMatrix;
			int mFrame;

			KeyFrame()
			{
				mMatrix.Identity();
				mFrame = 0;
			}
		};

		struct Object
		{
			Mesh** mMesh;
			UINT mNumMeshes;

			KeyFrame* mKeyFrame;
			UINT mNumKeyFrames;

			float mCurrTime;
			int mCurrKey;

			Matrix4x4 mMatrixWorld;
			Matrix4x4 mInverseBone;

			Object* mParent;

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
				SAFE_DELETE_ARRAY(mMesh);
				SAFE_DELETE_ARRAY(mKeyFrame);
			}
		};

		Object* mObject;
		UINT mNumObjects;

		std::shared_ptr<Material>* mMaterials;
		UINT mNumMaterials;

		bool mIsWeighted; // Are the vertices weighted?

		int mVersion;

	public:

		ModelM3D()
		{
			mObject = NULL;
			mNumObjects = 0;

			mMaterials = NULL;
			mNumMaterials = 0;

			mIsWeighted = false;

			mMatrixWorld.Identity();
		}

		~ModelM3D()
		{
			Release();
		}

		void Release()
		{
			for (UINT x = 0; x < mNumObjects; ++x)
			{
				for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
					SAFE_DELETE(mObject[x].mMesh[y]);

				SAFE_DELETE_ARRAY(mObject[x].mMesh);
				SAFE_DELETE_ARRAY(mObject[x].mKeyFrame);
			}

			SAFE_DELETE_ARRAY(mMaterials);
			SAFE_DELETE_ARRAY(mObject);

			mNumObjects = 0;
			mNumMaterials = 0;
		}

		void Save(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager)
		{
			// Save the format type.
			//
			BYTE format = M3D;
			archive.Write(&format);

			// Save if the model has weighted vertices.
			//
			BYTE weighted = mIsWeighted;
			archive.Write(&weighted);

			// Write the materials.
			//
			archive.Write(&mNumMaterials);

			for (UINT x = 0; x < mNumMaterials; ++x)
				WriteMaterial(archive, mMaterials[x], textureManager);

			// Write the objects.
			//
			archive.Write(&mNumObjects);

			for (UINT x = 0; x < mNumObjects; ++x)
			{
				// Write the meshes.
				//
				archive.Write(&mObject[x].mNumMeshes);

				for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
					Mesh::Save(archive, mObject[x].mMesh[y], renderer, shaderManager, textureManager, materialManager);

				// Write the keyframes.
				//
				archive.Write(&mObject[x].mNumKeyFrames);

				for (UINT y = 0; y < mObject[x].mNumKeyFrames; ++y)
				{
					archive.Write(mObject[x].mKeyFrame[y].mMatrix.Ptr(), 16);
					archive.Write(&mObject[x].mKeyFrame[y].mFrame);
				}

				// Write the hierarchy.
				//
				if (mObject[x].mParent == NULL)
				{
					int hierarchy = -1;
					archive.Write(&hierarchy);
				}
				else
				{
					for (int y = 0; y < (int)mNumObjects; ++y)
					{
						if (mObject[x].mParent == &mObject[y])
						{
							archive.Write(&y);
							break;
						}
					}
				}
			}
		}

		void CreateFromArchive(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager)
		{
			// Read if the model has weighted vertices.
			//
			BYTE weighted;
			archive.Read(&weighted);
			mIsWeighted = (weighted == 1);

			// Read the materials.
			//
			archive.Read(&mNumMaterials);
			mMaterials = new std::shared_ptr< Material >[mNumMaterials];

			for (UINT x = 0; x < mNumMaterials; ++x)
				ReadMaterial(archive, mMaterials[x], renderer, textureManager);

			// Read the objects.
			//
			archive.Read(&mNumObjects);
			mObject = new Object[mNumObjects];

			for (UINT x = 0; x < mNumObjects; ++x)
			{
				// Read the meshes.
				//
				archive.Read(&mObject[x].mNumMeshes);
				mObject[x].mMesh = new Mesh*[mObject[x].mNumMeshes];

				for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
					mObject[x].mMesh[y] = Mesh::Load(archive, renderer, shaderManager, textureManager, materialManager);

				// Read the keyframes.
				//
				archive.Read(&mObject[x].mNumKeyFrames);
				mObject[x].mKeyFrame = new KeyFrame[mObject[x].mNumKeyFrames];

				for (UINT y = 0; y < mObject[x].mNumKeyFrames; ++y)
				{
					archive.Read(mObject[x].mKeyFrame[y].mMatrix.Ptr(), 16);
					archive.Read(&mObject[x].mKeyFrame[y].mFrame);
				}

				// Read the hierarchy.
				//
				int hierarchy;
				archive.Read(&hierarchy);

				if (hierarchy == -1)
					mObject[x].mParent = NULL;
				else
					mObject[x].mParent = &mObject[hierarchy];
			}
		}

		bool CreateFromFile(
			const char* filename,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager)
		{
			Vertex* vertices = NULL;
			Vector3* normals = NULL;
			Vector2* texCoords = NULL;

			try
			{
				Archive archive;
				if (!archive.Open(filename, "rb"))
					throw AppException("Failed to load " + std::string(filename));

				// Read version.
				//
				archive.Read(&mVersion);

				// Read the materials and create meshes.
				//
				archive.Read(&mNumMaterials);
				mMaterials = new std::shared_ptr< Material >[mNumMaterials];

				for (UINT x = 0; x < mNumMaterials; ++x)
				{
					mMaterials[x] = std::shared_ptr< Material >(new Material());

					ReadMaterial(archive, mMaterials[x], renderer, textureManager);
				}

				// Read whether any data was exported using 32-bit.
				//
				const BYTE WEIGHTED = 0x01;
				const BYTE VERTS_32 = 0x02;
				const BYTE NORMS_32 = 0x04;
				const BYTE TEXCS_32 = 0x08;
				const BYTE INDEX_32 = 0x10;

				BYTE export32;
				archive.Read(&export32);

				bool bVerts32 = (export32 & VERTS_32) ? true : false;
				bool bNorms32 = (export32 & NORMS_32) ? true : false;
				bool bTexcs32 = (export32 & TEXCS_32) ? true : false;
				bool bIndex32 = (export32 & INDEX_32) ? true : false;

				mIsWeighted = (export32 & WEIGHTED) ? true : false;

				// Read vertices.
				//
				int numVerts;
				archive.Read(&numVerts, 1, bVerts32);
				vertices = new Vertex[numVerts];

				for (int x = 0; x < numVerts; ++x)
				{
					archive.Read(vertices[x].mPosition.Ptr(), 3);

					if (mIsWeighted)
					{
						archive.Read(&vertices[x].mBoneCount);

						for (int y = 0; y < vertices[x].mBoneCount; ++y)
						{
							archive.Read(&vertices[x].mBoneIndex[y]);
							archive.Read(&vertices[x].mBoneWeight[y]);
						}
					}
				}

				// Read normals.
				//
				int numNormals;
				archive.Read(&numNormals, 1, bNorms32);
				normals = new Vector3[numNormals];

				for (int x = 0; x < numNormals; ++x)
					archive.Read(normals[x].Ptr(), 3);

				// Read texture coordinates.
				//
				int numTexCoords;
				archive.Read(&numTexCoords, 1, bTexcs32);
				texCoords = new Vector2[numTexCoords];

				for (int x = 0; x < numTexCoords; ++x)
				{
					archive.Read(texCoords[x].Ptr(), 2);
					texCoords[x].y = 1.0f - texCoords[x].y;
				}

				// Read fat indices.
				//
				archive.Read(&mNumObjects);
				mObject = new Object[mNumObjects];
				int currHierarchy = 0;

				for (UINT x = 0; x < mNumObjects; ++x)
				{
					// Read the hierarchy number.
					//
					int hierarchy;
					archive.Read(&hierarchy);

					if (hierarchy > 0)
						mObject[x].mParent = &mObject[currHierarchy + hierarchy - 1];
					else
						currHierarchy = x;

					// Read if this object is skinned.
					//
					BYTE isSkinned;
					archive.Read(&isSkinned);

					// Read the keyframe animations.
					//
					archive.Read(&mObject[x].mNumKeyFrames);
					mObject[x].mKeyFrame = new KeyFrame[mObject[x].mNumKeyFrames];

					for (UINT y = 0; y < mObject[x].mNumKeyFrames; ++y)
					{
						KeyFrame* currKey = &mObject[x].mKeyFrame[y];

						// Read matrix.
						//
						archive.Read(currKey->mMatrix.Ptr(), 16);

						// Read frame timestamp.
						//
						archive.Read(&currKey->mFrame);
					}

					// Prepare keyframes for skinned animation if necessary.
					//
					if (mIsWeighted)
						mObject[x].mInverseBone = mObject[x].mKeyFrame[0].mMatrix.Inverse();

					// Read indices.
					//
					archive.Read(&mObject[x].mNumMeshes);
					mObject[x].mMesh = new Mesh*[mObject[x].mNumMeshes];

					for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
						mObject[x].mMesh[y] = NULL;

					MeshBuilder builder;

					for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
					{
						builder.ClearAll();

						// Count the number of textures used.
						//
						UINT numTextures = 0;

						// Read the material ID for this object.
						//
						std::shared_ptr< Material > material;

						int matID;
						archive.Read(&matID);

						if (matID != -1)
						{
							material = mMaterials[matID];

							for (UINT z = 0; z < TextureIndex::COUNT; ++z)
								if (material->mTexture[z].get() != NULL)
									++numTextures;
						}
						else
						{
							material = std::shared_ptr< Material >(new Material());
						}

						std::shared_ptr< Shader > shader;

						if (numTextures == 0)
						{
							shader = shaderManager->Get("Color_Lit");
						}
						else if (isSkinned)
						{
							shader = shaderManager->Get("Skinned");
						}
						else if (numTextures == 1)
						{
							shader = shaderManager->Get("Texture_Lit");
						}
						else if (numTextures == 2)
						{
							shader = shaderManager->Get("Normal_Map");
						}
						else
						{
							shader = shaderManager->Get("Parallax");
						}

						material->mShader = shader;

						builder.mLayout = shader->Layout();

						// Read faces.
						//
						MeshBuilder::Vertex meshVertex;

						int count;
						archive.Read(&count, 1, bIndex32);

						for (int z = 0; z < count; ++z)
						{
							int vertex;
							archive.Read(&vertex, 1, bVerts32);

							int normal;
							archive.Read(&normal, 1, bNorms32);

							int texCoord;
							archive.Read(&texCoord, 1, bTexcs32);

							meshVertex.mPosition = vertices[vertex].mPosition;

							if (isSkinned)
							{
								COPY_ARRAY(meshVertex.mBoneWeight, vertices[vertex].mBoneWeight, 4);
								COPY_ARRAY(meshVertex.mBoneIndex, vertices[vertex].mBoneIndex, 4);
								meshVertex.mBoneCount = vertices[vertex].mBoneCount;
							}

							meshVertex.mNormal = normals[normal];
							meshVertex.mTexCoord[0] = Vector2(texCoords[texCoord].x, texCoords[texCoord].y);

							builder.mVertex.push_back(meshVertex);

							builder.AddIndex(z);
						}

						builder.CalculateTangents(false);

						mObject[x].mMesh[y] = builder.BuildMesh(renderer);
						mObject[x].mMesh[y]->mMaterial = material;

						builder.ApplyMatrix(mObject[x].mKeyFrame[0].mMatrix);

						builder.ClearAll();
					}
				}

				delete[] vertices;
				delete[] normals;
				delete[] texCoords;

				archive.Close();
			}
			catch (AppException e)
			{
				Debug::ShowError(e.what(), "Model Load Failure");

				SAFE_DELETE_ARRAY(vertices);
				SAFE_DELETE_ARRAY(normals);
				SAFE_DELETE_ARRAY(texCoords);

				Release();

				return false;
			}

			return true;
		}

	private:

		void WriteMaterial(
			Archive& archive,
			std::shared_ptr<Material> material,
			TextureManager* textureManager)
		{
			// Write base material.
			//
			archive.Write(material->mAmbient.Ptr(), 3);
			archive.Write(material->mDiffuse.Ptr(), 3);
			archive.Write(material->mSpecular.Ptr(), 3);

			float spec_comp = material->mSpecularComponent / 100.0f;
			archive.Write(&spec_comp);

			float alpha = material->mAmbient.a + material->mDiffuse.a + material->mSpecular.a;
			archive.Write(&alpha);

			// Write filenames of each texture.
			//
			UINT numTextures = 0;
			for (UINT x = 0; x < NUM_AUTODESK_TYPES; ++x)
				if (material->mTexture[x] != NULL)
					++numTextures;

			archive.Write(&numTextures, 1, false);

			for (BYTE x = 0; x < NUM_AUTODESK_TYPES; ++x)
			{
				if (material->mTexture[x] != NULL)
				{
					archive.Write(&x);

					std::string name = textureManager->Get(material->mTexture[x]);

					archive.Write(&name);
				}
			}
		}

		void ReadMaterial(
			Archive& archive,
			std::shared_ptr<Material> material,
			Renderer* renderer,
			TextureManager* textureManager)
		{
			// Set the material.
			//
			Vector3 ambient;
			archive.Read(ambient.Ptr(), ar_sizeof(ambient));

			Vector3 diffuse;
			archive.Read(diffuse.Ptr(), ar_sizeof(diffuse));

			Vector3 specular;
			archive.Read(specular.Ptr(), ar_sizeof(specular));

			float spec_comp;
			archive.Read(&spec_comp);
			spec_comp *= 100.0f;

			float alpha;
			archive.Read(&alpha);

			material->mAmbient = ColorRGBA(ambient.x, ambient.y, ambient.z, alpha);
			material->mDiffuse = ColorRGBA(diffuse.x, diffuse.y, diffuse.z, 0);
			material->mSpecular = ColorRGBA(specular.x, specular.y, specular.z, 0);
			material->mSpecularComponent = spec_comp;

			if (alpha < 0.98f) // close enough
				material->mBlendState = renderer->BLEND_ALPHA;
			else
				material->mBlendState = renderer->BLEND_OFF;

			// Read filenames of each texture.
			//
			UINT numTextures;
			archive.Read(&numTextures, 1, false);

			for (UINT x = 0; x < numTextures; ++x)
			{
				BYTE type;
				archive.Read(&type);

				switch (type)
				{
				case AUTODESK_BUMP:
					type = TextureIndex::NORMAL;
					break;

				case AUTODESK_PARALLAX:
					type = TextureIndex::DEPTH;
					break;

				default:
					type = TextureIndex::DIFFUSE;
					break;
				}

				std::string name;
				archive.Read(&name);

				material->mTexture[type] = textureManager->Add(name, std::shared_ptr< Texture >(renderer->CreateTextureFromFile(name.c_str())));
			}
		}

	public:

		void GetMeshList(std::vector< Mesh* >* meshList)
		{
			meshList->clear();

			for (UINT x = 0; x < mNumObjects; ++x)
				for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
					meshList->push_back(mObject[x].mMesh[y]);
		}

		void SetTime(float _time, UINT objIndex = 0)
		{
			_ASSERT(objIndex < mNumObjects);

			if ((UINT)_time < mObject[objIndex].mNumKeyFrames)
			{
				mObject[objIndex].mCurrTime = _time;
				mObject[objIndex].mCurrKey = (int)_time;
			}
			else
			{
				mObject[objIndex].mCurrTime = 0;
				mObject[objIndex].mCurrKey = 0;
			}
		}

		float GetTime(UINT objIndex = 0)
		{
			_ASSERT(objIndex < mNumObjects);

			return mObject[objIndex].mCurrTime;
		}

		void Update(float DT)
		{
			// Setup Bone Matrix.
			//
			/*
			if( mIsWeighted )
			{
			static Matrix4x4 matBone;

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

			for (UINT x = 0; x < mNumObjects; ++x)
			{
				// Adjust keyframe for animation times.
				//
				if (mObject[x].mNumKeyFrames > 0)
				{
					mObject[x].mCurrTime += DT * 30.0f;

					if (mObject[x].mKeyFrame[mObject[x].mCurrKey].mFrame <= (int)mObject[x].mCurrTime)
					{
						++mObject[x].mCurrKey;

						if ((int)mObject[x].mCurrKey >= (int)mObject[x].mNumKeyFrames)
						{
							mObject[x].mCurrTime = 0;
							mObject[x].mCurrKey = 0;
						}
					}
				}
			}
		}

		void Draw(Renderer* renderer, GameWorld* world, Component::Camera* camera)
		{
			// Set up model transforms.
			//
			for (UINT x = 0; x < mNumObjects; ++x)
			{
				static Matrix4x4 matWorldObject;

				if (mObject[x].mParent == NULL)
					matWorldObject = mMatrixWorld * mObject[x].mKeyFrame[mObject[x].mCurrKey].mMatrix;
				else
					matWorldObject = mObject[x].mParent->mMatrixWorld * mObject[x].mKeyFrame[mObject[x].mCurrKey].mMatrix;

				mObject[x].mMatrixWorld = matWorldObject;

				// Render each mesh by material.
				//
				for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
				{
					mObject[x].mMesh[y]->mMatrixWorld = matWorldObject;
					mObject[x].mMesh[y]->Draw(renderer, world, camera);
				}
			}
		}
	};

	// Create an M3D Model.
	//
	Model* LoadModelM3DFromFile(
		const char* filename,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager)
	{
		ModelM3D* model = new ModelM3D();

		model->CreateFromFile(filename, renderer, shaderManager, textureManager, materialManager);

		return model;
	}

	Model* LoadModelM3DFromArchive(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager)
	{
		ModelM3D* model = new ModelM3D();

		model->CreateFromArchive(archive, renderer, shaderManager, textureManager, materialManager);

		return model;
	}
}
