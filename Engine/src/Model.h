#pragma once
/*
Loads a model from a file into a set of meshes.
The underlying implementation is hidden within
separate source files because the user should
never need to know the model format, just that
it contains geometry to be rendered.

The OBJ file format does not contain keyframes;
therefore, calling the time functions have
no effect.

Add shaders to the ShaderManager with the
following string designations before loading
a model:

// Color with normal
ShaderManager::Add(shader, "Color_Lit");

// Texture with normal
ShaderManager::Add(shader, "Texture_Lit");

// Normal mapping
ShaderManager::Add(shader, "Normal_Map");

// Parallax mapping
ShaderManager::Add(shader, "Parallax");

// Skinning and bones
ShaderManager::Add(shader, "Skinned");

The previous shaders are created and loaded
automatically through the editor using the
'config.xml' file located within the
'Assets\Shaders' folder.

Each Model has self contained Materials.
*/
#include <vector>

#include "Matrix4x4.h"

namespace Sentinel
{
	namespace Component
	{
		class Camera;
	}

	class Archive;
	class Renderer;
	class ShaderManager;
	class TextureManager;
	class MaterialManager;
	class BlendStateManager;
	class Shader;
	class Mesh;
	class GameWorld;

	class SENTINEL_DLL Model
	{
	protected:
		enum Format
		{
			INVALID,
			OBJ,
			M3D,
		};

	public:
		Matrix4x4 mMatrixWorld;

		/////////////////////////////////////

		virtual ~Model();

		static Model* Load(
			const char* filename,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager,
			BlendStateManager* blendManager);

		virtual void Save(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager,
			BlendStateManager* blendManager) = 0;

		static Model* Load(
			Archive& archive,
			Renderer* renderer,
			ShaderManager* shaderManager,
			TextureManager* textureManager,
			MaterialManager* materialManager,
			BlendStateManager* blendManager);

		/////////////////////////////////////

		virtual void GetMeshList(std::vector<Mesh*>* meshList) = 0;

		virtual void SetTime(float _time, UINT objIndex = 0) = 0;
		virtual float GetTime(UINT objIndex = 0) = 0;
		virtual void Update(float DT = 0) = 0;

		virtual void Draw(Renderer* renderer, GameWorld* world, Component::Camera* camera) = 0;
	};

	extern SENTINEL_DLL Model* LoadModelOBJFromFile(
		const char* filename,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager);

	extern SENTINEL_DLL Model* LoadModelM3DFromFile(
		const char* filename,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager);

	// The Archive format is different than the native file format.
	// It is designed specifically to be read quickly and easily
	// by the engine.
	//
	// Save() creates a single BYTE header designating what the
	// format of the following data represents. Use Model::Load()
	// to create the Model with the correct format.
	//
	extern SENTINEL_DLL Model* LoadModelOBJFromArchive(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager);

	extern SENTINEL_DLL Model* LoadModelM3DFromArchive(
		Archive& archive,
		Renderer* renderer,
		ShaderManager* shaderManager,
		TextureManager* textureManager,
		MaterialManager* materialManager,
		BlendStateManager* blendManager);
}
