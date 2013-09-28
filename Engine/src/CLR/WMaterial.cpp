#include "WMaterial.h"
#include "ColorRGBA.h"
#include "Util.h"

namespace Sentinel { namespace Assets
{
	DEFINE_REF( Material );

	WMaterial::WMaterial()
	{
		mRef = new Material( ColorRGBA( 0.2f, 0.2f, 0.2f, 1.0f ),
							 ColorRGBA( 0.6f, 0.6f, 0.6f, 1.0f ),
							 ColorRGBA( 0.2f, 0.2f, 0.2f, 1.0f ),
							 32.0f );
	}

	WMaterial::WMaterial( WColorRGBA^ ambient,
						  WColorRGBA^ diffuse,
						  WColorRGBA^ specular,
						  float spec_comp )
	{
		mRef = new Material( *ambient->GetRef(), 
							 *diffuse->GetRef(), 
							 *specular->GetRef(), 
							 spec_comp );
	}

	WMaterial::WMaterial( Material& material )
	{
		mRef = new Material( material );
	}

	WMaterial::WMaterial( WMaterial^ material )
	{
		mRef = new Material( *material->mRef );
	}

	DEFINE_PROPERTY_R( Material, ColorRGBA, Ambient );
	DEFINE_PROPERTY_R( Material, ColorRGBA, Diffuse );
	DEFINE_PROPERTY_R( Material, ColorRGBA, Specular );
	DEFINE_PROPERTY_M( Material, float,		SpecularComponent );
	
	DEFINE_OP_DEREF( Material );

	////////////////////////////////

	DEFINE_CLASS_REF( Material );
}}
