#include "AssetManager.h"
#include "Model.h"

namespace Sentinel
{
	class SENTINEL_DLL ModelManager : public AssetManager< Model >, public SingletonSafe< ModelManager >
	{
		friend class SingletonSafe< ModelManager >;
		friend class AssetManager< Model >;
		
	private:

		ModelManager();
		~ModelManager();

	public:

		static ModelManager* Create();

		void Save( Archive& archive );

		void Load( Archive& archive );
	};
}
