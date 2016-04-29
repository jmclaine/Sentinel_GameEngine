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
#include "BlendStateManager.h"

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

			int mBoneCount;
			int mBoneIndex[4];
			float mBoneWeight[4];

			Vertex() : mBoneCount(0) { }
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
				mParent = nullptr;

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
			mObject = nullptr;
			mNumObjects = 0;

			mMaterials = nullptr;
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
			MaterialManager* materialManager,
			BlendStateManager* blendManager)
		{
			BYTE format = M3D;
			archive.Write(&format);

			BYTE weighted = mIsWeighted;
			archive.Write(&weighted);

			archive.Write(&mNumMaterials);

			for (UINT x = 0; x < mNumMaterials; ++x)
				WriteMaterial(archive, mMaterials[x], textureManager);

			archive.Write(&mNumObjects);

			for (UINT x = 0; x < mNumObjects; ++x)
			{
				archive.Write(&mObject[x].mNumMeshes);

				for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
					Mesh::Save(archive, mObject[x].mMesh[y], renderer, materialManager);

				archive.Write(&mObject[x].mNumKeyFrames);

				for (UINT y = 0; y < mObject[x].mNumKeyFrames; ++y)
				{
					archive.Write(&mObject[x].mKeyFrame[y].mMatrix);
					archive.Write(&mObject[x].mKeyFrame[y].mFrame);
				}

				if (mObject[x].mParent == nullptr)
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
			MaterialManager* materialManager,
			BlendStateManager* blendManager)
		{
			BYTE weighted;
			archive.Read(&weighted);
			mIsWeighted = (weighted == 1);

			archive.Read(&mNumMaterials);
			mMaterials = new std::shared_ptr<Material>[mNumMaterials];

			for (UINT x = 0; x < mNumMaterials; ++x)
				ReadMaterial(archive, mMaterials[x], renderer, textureManager, blendManager);

			archive.Read(&mNumObjects);
			mObject = new Object[mNumObjects];

			for (UINT x = 0; x < mNumObjects; ++x)
			{
				archive.Read(&mObject[x].mNumMeshes);
				mObject[x].mMesh = new Mesh*[mObject[x].mNumMeshes];

				for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
					mObject[x].mMesh[y] = Mesh::Load(archive, renderer, materialManager);

				archive.Read(&mObject[x].mNumKeyFrames);
				mObject[x].mKeyFrame = new KeyFrame[mObject[x].mNumKeyFrames];

				for (UINT y = 0; y < mObject[x].mNumKeyFrames; ++y)
				{
					archive.Read(&mObject[x].mKeyFrame[y].mMatrix);
					archive.Read(&mObject[x].mKeyFrame[y].mFrame);
				}

				int hierarchy;
				archive.Read(&hierarchy);

				if (hierarchy == -1)
					mObject[x].mParent = nullptr;
				else
					mObject[x].mParent = &mObject[hierarchy];
			}
		}

		bool CreateFromFile(
			const char* filename,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager,
			BlendStateManager* blendManager)
		{
			Vertex* vertices = nullptr;
			Vector3* normals = nullptr;
			Vector2* texCoords = nullptr;

			try
			{
				Archive archive;
				if (!archive.Open(filename, "rb"))
					throw AppException("Failed to load " + std::string(filename));

				archive.Read(&mVersion);

				archive.Read(&mNumMaterials);
				mMaterials = new std::shared_ptr<Material>[mNumMaterials];

				for (UINT x = 0; x < mNumMaterials; ++x)
				{
					mMaterials[x] = std::shared_ptr<Material>(new Material());

					ReadMaterial(archive, mMaterials[x], renderer, textureManager, blendManager);
				}

				BYTE import;
				archive.Read(&import);

				mIsWeighted = (import & 0x01) ? true : false;
				bool bVerts32 = ((import & 0x02) == 0);
				bool bNorms32 = ((import & 0x04) == 0);
				bool bTexcs32 = ((import & 0x08) == 0);
				bool bIndex32 = ((import & 0x10) == 0);

				int numVerts;
				if (bVerts32)
					archive.ReadConvert<int, short>(&numVerts);
				else
					archive.Read(&numVerts);
				vertices = new Vertex[numVerts];

				for (int x = 0; x < numVerts; ++x)
				{
					archive.Read(&vertices[x].mPosition);

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


				int numNormals;
				if (bNorms32)
					archive.ReadConvert<int, short>(&numNormals);
				else
					archive.Read(&numNormals);
				normals = new Vector3[numNormals];

				for (int x = 0; x < numNormals; ++x)
					archive.Read(&normals[x]);

				
				int numTexCoords;
				if (bTexcs32)
					archive.ReadConvert<int, short>(&numTexCoords);
				else
					archive.Read(&numTexCoords);
				texCoords = new Vector2[numTexCoords];

				for (int x = 0; x < numTexCoords; ++x)
				{
					archive.Read(&texCoords[x]);
					texCoords[x].y = 1.0f - texCoords[x].y;
				}


				archive.Read(&mNumObjects);
				mObject = new Object[mNumObjects];
				int currHierarchy = 0;

				for (UINT x = 0; x < mNumObjects; ++x)
				{
					int hierarchy;
					archive.Read(&hierarchy);

					if (hierarchy > 0)
						mObject[x].mParent = &mObject[currHierarchy + hierarchy - 1];
					else
						currHierarchy = x;

					
					BYTE isSkinned;
					archive.Read(&isSkinned);

					
					archive.Read(&mObject[x].mNumKeyFrames);
					mObject[x].mKeyFrame = new KeyFrame[mObject[x].mNumKeyFrames];

					for (UINT y = 0; y < mObject[x].mNumKeyFrames; ++y)
					{
						KeyFrame* currKey = &mObject[x].mKeyFrame[y];

						archive.Read(&currKey->mMatrix);
						archive.Read(&currKey->mFrame);
					}

					if (mIsWeighted)
						mObject[x].mInverseBone = mObject[x].mKeyFrame[0].mMatrix.Inverse();

					archive.Read(&mObject[x].mNumMeshes);
					mObject[x].mMesh = new Mesh*[mObject[x].mNumMeshes];

					for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
						mObject[x].mMesh[y] = nullptr;

					MeshBuilder builder;
					for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
					{
						builder.ClearAll();

						BYTE numTextures = 0;

						std::shared_ptr<Material> material;

						int matID;
						archive.Read(&matID);

						if (matID != -1)
						{
							material = mMaterials[matID];

							for (UINT z = 0; z < (UINT)TextureIndex::COUNT; ++z)
								if (material->mTexture[z].lock() != nullptr)
									++numTextures;
						}
						else
						{
							material = std::shared_ptr<Material>(new Material());
						}

						if (numTextures == 0)
						{
							material->mShader = shaderManager->Get("Color_Lit");
						}
						else if (isSkinned)
						{
							material->mShader = shaderManager->Get("Skinned");
						}
						else if (numTextures == 1)
						{
							material->mShader = shaderManager->Get("Texture_Lit");
						}
						else if (numTextures == 2)
						{
							material->mShader = shaderManager->Get("Normal_Map");
						}
						else
						{
							material->mShader = shaderManager->Get("Parallax");
						}

						builder.mLayout = material->mShader.lock()->Layout();

						
						MeshBuilder::Vertex meshVertex;

						int count;
						if (bIndex32)
							archive.ReadConvert<int, short>(&count);
						else
							archive.Read(&count);

						for (int z = 0; z < count; ++z)
						{
							int vertex;
							if (bVerts32)
								archive.ReadConvert<int, short>(&vertex);
							else
								archive.Read(&vertex);

							int normal;
							if (bNorms32)
								archive.ReadConvert<int, short>(&normal);
							else
								archive.Read(&normal);

							int texCoord;
							if (bTexcs32)
								archive.ReadConvert<int, short>(&texCoord);
							else
								archive.Read(&texCoord);

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
			archive.Write(&material->mAmbient);
			archive.Write(&material->mDiffuse);
			archive.Write(&material->mSpecular);

			float spec_comp = material->mSpecularComponent / 100.0f;
			archive.Write(&spec_comp);

			float alpha = material->mAmbient.a + material->mDiffuse.a + material->mSpecular.a;
			archive.Write(&alpha);

			
			USHORT numTextures = 0;
			for (UINT x = 0; x < (UINT)NUM_AUTODESK_TYPES; ++x)
				if (material->mTexture[x].lock() != nullptr)
					++numTextures;

			archive.Write(&numTextures);

			for (BYTE x = 0; x < (BYTE)NUM_AUTODESK_TYPES; ++x)
			{
				if (material->mTexture[x].lock() != nullptr)
				{
					archive.Write(&x);

					std::string name = textureManager->Get(material->mTexture[x]);

					archive.Write(&name);
				}
			}
		}

		void ReadMaterial(
			Archive& archive,
			std::shared_ptr<Material>& material,
			Renderer* renderer,
			TextureManager* textureManager,
			BlendStateManager* blendManager)
		{
			Vector3 ambient;
			archive.Read(&ambient);

			Vector3 diffuse;
			archive.Read(&diffuse);

			Vector3 specular;
			archive.Read(&specular);

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
				material->mBlendState = blendManager->Get("ALPHA");
			else
				material->mBlendState = blendManager->Get("DEFAULT");

			
			USHORT numTextures;
			archive.Read(&numTextures);

			for (USHORT x = 0; x < numTextures; ++x)
			{
				BYTE type;
				archive.Read(&type);

				switch (type)
				{
				case AUTODESK_BUMP:
					type = (BYTE)TextureIndex::NORMAL;
					break;

				case AUTODESK_PARALLAX:
					type = (BYTE)TextureIndex::DEPTH;
					break;

				default:
					type = (BYTE)TextureIndex::DIFFUSE;
					break;
				}

				std::string name;
				archive.Read(&name);

				auto texture = std::shared_ptr<Texture>(renderer->CreateTextureFromFile(name.c_str()));

				if (texture.get() == nullptr)
				{
					Debug::ShowError(
						STREAM("Failed to load image '" << name << "'"),
						STREAM("Load Model Error"));

					return;
				}
				textureManager->Add(name, texture);

				material->mTexture[type] = textureManager->Get(name);
			}
		}

	public:

		void GetMeshList(std::vector<Mesh*>* meshList)
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
			/*
			if (mIsWeighted)
			{
				static Matrix4x4 matBone;

				std::shared_ptr<Shader> skin = ShaderManager::Inst()->Get("Skinning");

				Renderer::Inst()->SetShader(skin);

				for (UINT x = 0; x < mNumObjects; ++x)
				{
					matBone = mObject[x].mKeyFrame[mObject[x].mCurrKey].mMatrix * mObject[x].mInverseBone;

					// Set the B designation first to ensure this works correctly.
					//
					skin->SetMatrix(skin->Uniform().find('B'), matBone.Ptr(), x, 1);
				}
			}
			*/

			static Object* obj;

			for (UINT x = 0; x < mNumObjects; ++x)
			{
				obj = &mObject[x];

				if (obj->mNumKeyFrames > 0)
				{
					obj->mCurrTime += DT * 30.0f;

					if (obj->mKeyFrame[obj->mCurrKey].mFrame <= (int)obj->mCurrTime)
					{
						++obj->mCurrKey;

						if (obj->mCurrKey >= (int)obj->mNumKeyFrames)
						{
							obj->mCurrTime = 0;
							obj->mCurrKey = 0;
						}
					}
				}
			}

			for (UINT x = 0; x < mNumObjects; ++x)
			{
				static Matrix4x4 matWorldObject;
				
				obj = &mObject[x];

				if (obj->mParent == nullptr)
					matWorldObject = mMatrixWorld * obj->mKeyFrame[obj->mCurrKey].mMatrix;
				else
					matWorldObject = obj->mParent->mMatrixWorld * obj->mKeyFrame[obj->mCurrKey].mMatrix;

				obj->mMatrixWorld = matWorldObject;

				for (UINT y = 0; y < mObject[x].mNumMeshes; ++y)
				{
					obj->mMesh[y]->mMatrixWorld = matWorldObject;
				}
			}
		}

		void Draw(Renderer* renderer, GameWorld* world, Component::Camera* camera)
		{
			static Object* obj;

			for (UINT x = 0; x < mNumObjects; ++x)
			{
				obj = &mObject[x];

				for (UINT y = 0; y < obj->mNumMeshes; ++y)
				{
					obj->mMesh[y]->Draw(renderer, world, camera);
				}
			}
		}
	};

	Model* LoadModelM3DFromFile(
		const char* filename,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager)
	{
		ModelM3D* model = new ModelM3D();

		model->CreateFromFile(filename, renderer, shaderManager, textureManager, materialManager, blendManager);

		return model;
	}

	Model* LoadModelM3DFromArchive(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager)
	{
		ModelM3D* model = new ModelM3D();

		model->CreateFromArchive(archive, renderer, shaderManager, textureManager, materialManager, blendManager);

		return model;
	}
}
