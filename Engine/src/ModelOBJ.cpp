#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include "Debug.h"
#include "Model.h"
#include "MeshBuilder.h"
#include "Util.h"
#include "Renderer.h"
#include "Component/Camera.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "Archive.h"
#include "MathUtil.h"

namespace Sentinel
{
	class ModelOBJ : public Model
	{
	private:
		typedef std::unordered_map<std::string, MeshBuilder*> MeshBuilderMap;
		typedef std::pair<std::string, MeshBuilder*> MeshBuilderPair;

		Mesh** mMesh;
		UINT mNumMeshes;

	public:
		ModelOBJ()
		{
			mMesh = nullptr;
			mNumMeshes = 0;
		}

		~ModelOBJ()
		{
			for (UINT x = 0; x < mNumMeshes; ++x)
				delete mMesh[x];

			delete[] mMesh;
		}

		void Save(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager,
			BlendStateManager* blendManager)
		{
			// Save the format type.
			//
			BYTE format = OBJ;
			archive.Write(&format);

			// Save each mesh.
			//
			archive.Write(&mNumMeshes);

			for (UINT x = 0; x < mNumMeshes; ++x)
				Mesh::Save(archive, mMesh[x], renderer, materialManager);
		}

		void Create(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager)
		{
			// Load each mesh.
			//
			archive.Read(&mNumMeshes);

			mMesh = new Mesh*[mNumMeshes];

			for (UINT x = 0; x < mNumMeshes; ++x)
				mMesh[x] = Mesh::Load(archive, renderer, materialManager);
		}

		bool Create(
			const char* filename,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager)
		{
			// Model data.
			// OBJ files start with the first index as 1,
			// so push a dummy into the vectors to align.
			//
			std::vector<Vector3> positions(1);
			std::vector<Vector2> texCoords(1);
			std::vector<Vector3> normals(1);

			// Initialize the minimum and maximum vertex positions for the bounding sphere.
			//
			Vector3 minPosition(FLT_MAX, FLT_MAX, FLT_MAX);
			Vector3 maxPosition(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			// Create a default material.
			//
			MeshBuilderMap builder;
			const std::string defaultMaterialName = "~*Default*~";
			builder.insert(MeshBuilderPair(defaultMaterialName, new MeshBuilder()));

			auto shaderColor = shaderManager->Get("Color").lock();
			auto shaderTexture = shaderManager->Get("Texture").lock();

			MeshBuilder* meshBuilder = builder.begin()->second;

			// Read the file.
			//
			std::string line;
			std::ifstream file(filename);

			if (file.is_open())
			{
				while (file.good())
				{
					getline(file, line);

					std::stringstream parsehelper(line);
					std::string token;

					parsehelper >> token;

					// Comment.
					//
					if (token == "#")
					{
						continue;
					}
					// Vertex.
					//
					else if (token == "v")
					{
						Vector3 p;
						parsehelper >> p.x >> p.y >> p.z;
						positions.push_back(p);

						for (UINT x = 0; x < 3; ++x)
						{
							maxPosition[x] = std::fmax(maxPosition[x], p[x]);
							minPosition[x] = std::fmin(minPosition[x], p[x]);
						}
					}
					// Texture coordinates.
					//
					else if (token == "vt")
					{
						Vector2 p;
						parsehelper >> p.x >> p.y;
						p.y = 1 - p.y;				// Texture is reverse y-axis.
						texCoords.push_back(p);
					}
					// Vertex normals.
					//
					else if (token == "vn")
					{
						Vector3 p;
						parsehelper >> p.x >> p.y >> p.z;
						normals.push_back(p);
					}
					// Material.
					//
					else if (token == "mtllib")
					{
						// The next token is the filename.
						//
						parsehelper >> token;

						// Begin parsing through this material file.
						//
						std::string mtlLine;
						std::string mtlName;
						std::ifstream mtlFile(token.c_str());

						if (mtlFile.is_open())
						{
							while (mtlFile.good())
							{
								getline(mtlFile, mtlLine);

								std::stringstream mtlParsehelper(mtlLine);
								std::string mtlToken;

								mtlParsehelper >> mtlToken;

								if (mtlToken == "newmtl")
								{
									mtlParsehelper >> mtlName;
									builder.insert(MeshBuilderPair(mtlName, new MeshBuilder()));

									MeshBuilder* meshBuilder = builder[mtlName];

									meshBuilder->mLayout = shaderColor->Layout();
								}
								else if (mtlToken == "map_Kd")
								{
									const MeshBuilderMap::iterator& mtlIter = builder.find(mtlName);

									if (mtlIter != builder.end())
									{
										mtlParsehelper >> mtlToken;

										auto texture = std::shared_ptr<Texture>(renderer->CreateTextureFromFile(mtlToken.c_str()));

										if (texture.get() == nullptr)
										{
											Debug::ShowError(
												STREAM("Failed to load image '" << mtlToken << "'"), 
												STREAM("Load Model Error"));

											return false;
										}
										textureManager->Add(mtlName, texture);

										MeshBuilder* meshBuilder = mtlIter->second;
										meshBuilder->mLayout = shaderTexture->Layout();
									}
								}
							}
						}
					}
					else if (token == "usemtl")
					{
						std::string mtlName;
						parsehelper >> mtlName;

						const MeshBuilderMap::iterator& mtl_iter = builder.find(mtlName);

						if (mtl_iter == builder.end())
							meshBuilder = builder[defaultMaterialName];
						else
							meshBuilder = mtl_iter->second;
					}
					else if (token == "f")
					{
						if (meshBuilder == nullptr)
						{
							Debug::ShowError(
								"Invalid syntax for OBJ file.", 
								"OBJ Load Error");

							return false;
						}

						MeshBuilder::Vertex meshVertex;
						UINT i = 0;

						for (;;)
						{
							parsehelper >> token;

							if (parsehelper.fail())
								break;

							// Position
							// Texture coord
							// Normal
							
							int vIndex[3];
							int currType = 0;
							std::string fatIndex;

							for (UINT x = 0; x < token.size(); ++x)
							{
								if (token[x] != '/')
								{
									fatIndex.push_back(token[x]);
								}
								else
								{
									vIndex[currType] = atoi(fatIndex.c_str());
									fatIndex.clear();
									++currType;
								}
							}
							vIndex[currType] = atoi(fatIndex.c_str());

							UINT currVertex = meshBuilder->FindVertex(positions[vIndex[0]], texCoords[vIndex[1]], normals[vIndex[2]]);
							if (currVertex == UINT_MAX)
							{
								currVertex = meshBuilder->mVertex.size();

								meshVertex.mPosition = positions[vIndex[0]];
								meshVertex.mTexCoord[0] = texCoords[vIndex[1]];
								meshVertex.mNormal = normals[vIndex[2]];
								meshBuilder->mVertex.push_back(meshVertex);
							}

							meshBuilder->mIndex.push_back(currVertex);
							++i;

							if (i > 3)
							{
								UINT index = meshBuilder->mIndex.size();
								meshBuilder->mIndex.push_back(meshBuilder->mIndex[index - 4]);
								meshBuilder->mIndex.push_back(meshBuilder->mIndex[index - 2]);
							}
						}
					}
				}

				file.close();

				_ASSERT(0);	// requires a Material be created

				UINT i = 0;
				mMesh = new Mesh*[builder.size()];

				TRAVERSE_LIST(it, builder)
				{
					if (it->second->mVertex.size() > 0)
					{
						mMesh[i] = it->second->BuildMesh(renderer);
						++i;
					}

					delete it->second;
				}

				builder.clear();
			}
			else
			{
				Debug::ShowError(
					STREAM("Failed to load " << filename), 
					STREAM("OBJ Model Load Failure"));

				return false;
			}

			return true;
		}

		//////////////////////////////////////////////////////////////////////////

		void GetMeshList(std::vector<Mesh*>* meshList)
		{
			for (UINT x = 0; x < mNumMeshes; ++x)
				meshList->push_back(mMesh[x]);
		}

		// OBJ files do not support animation.
		//
		void SetTime(float _time, UINT objIndex = 0)
		{
			_ASSERT(0);	// unsupported
		}

		float GetTime(UINT objIndex = 0)
		{
			_ASSERT(0);	// unsupported
			return 0;
		}

		void Update(float DT)
		{
			_ASSERT(0);	// unsupported
		}

		// Render the model.
		//
		void Draw(Renderer* renderer, GameWorld* world, Component::Camera* camera)
		{
			for (UINT x = 0; x < mNumMeshes; ++x)
			{
				mMesh[x]->mMatrixWorld = mMatrixWorld;
				mMesh[x]->Draw(renderer, world, camera);
			}
		}
	};

	// Create an OBJ Model.
	//
	Model* LoadModelOBJFromFile(
		const char* filename,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager)
	{
		ModelOBJ* model = new ModelOBJ();

		model->Create(filename, renderer, shaderManager, textureManager, materialManager);

		return model;
	}

	Model* LoadModelOBJFromArchive(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager)
	{
		ModelOBJ* model = new ModelOBJ();

		model->Create(archive, renderer, shaderManager, textureManager, materialManager);

		return model;
	}
}
