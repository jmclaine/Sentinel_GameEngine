#include "Material.h"

namespace Sentinel
{
	Material::Material( const ColorRGBA& ambient,
						const ColorRGBA& diffuse,
						const ColorRGBA& specular,
						float spec_comp ) :
		mAmbient( ambient ), mDiffuse( diffuse ), mSpecular( specular ), mSpecularComponent( spec_comp )
	{}

	float* Material::Ptr()
	{
		return mAmbient.Ptr();
	}
}
