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
ShaderManager::Inst()->Add( shader, "Color" );

// Texture with normal
ShaderManager::Inst()->Add( shader, "Texture" );

// Normal mapping
ShaderManager::Inst()->Add( shader, "Normal Map" );

// Parallax mapping
ShaderManager::Inst()->Add( shader, "Parallax" );

// Skinning and bones
ShaderManager::Inst()->Add( shader, "Skinning" );
*/
#include <vector>

#include "Common.h"
#include "ShaderManager.h"
#include "Mesh.h"

namespace Sentinel
{
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

		Matrix4f		mMatrixWorld;

		/////////////////////////////////////

		virtual ~Model();
		
		static Model*	Load( const char* filename );
		
		// The Archive format is specifically created to make 
		// the model files easy to save and load for the engine.
		//
		virtual void	Save( Archive& archive ) = 0;
		static Model*	Load( Archive& archive );

		/////////////////////////////////////

		virtual void	Release() = 0;

		// There is no way to tell which materials pertain to what portion of the model.
		// A name label for each material in conjuction with a std::map would solve this problem.
		//
		virtual void	SetMaterials( const std::vector< Material >& material ) = 0;
		virtual void	GetMaterials( std::vector< Material >* material ) = 0;

		virtual void	SetTime( float _time, UINT objIndex = 0 ) = 0;
		virtual float	GetTime( UINT objIndex = 0 ) = 0;
		virtual void	Update() = 0;

		virtual void	Draw() = 0;
	};

	extern Model* LoadModelOBJFromFile( const char* filename );
	extern Model* LoadModelM3DFromFile( const char* filename );

	// The Archive format is different than the native file format.
	// It is designed specifically to be read quickly and easily
	// by the engine, and is the inverse of Save().
	//
	// Save() creates a single BYTE header designating what the
	// format of the following data represents. Use Model::Load()
	// to create the Model with the correct format.
	//
	extern Model* LoadModelOBJFromArchive( Archive& archive );
	extern Model* LoadModelM3DFromArchive( Archive& archive );
}
