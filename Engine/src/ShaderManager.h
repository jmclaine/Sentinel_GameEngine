#pragma once

#include "Common.h"
#include "AssetManager.h"
#include "Shader.h"
#include "Singleton.h"

namespace Sentinel
{
	class SENTINEL_DLL ShaderManager : public AssetManager< Shader >, public SingletonSafe< ShaderManager >
	{
		friend class SingletonSafe< ShaderManager >;
		friend class AssetManager< Shader >;

	private:

		ShaderManager();
		~ShaderManager();

	public:

		static ShaderManager* Create();

		void Save( Archive& archive );

		void Load( Archive& archive );

		bool LoadConfig( const char* filename );
	};
}
