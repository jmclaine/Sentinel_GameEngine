#pragma once

#include "Material.h"
#include "WColorRGBA.h"
#include "WStdVector.h"

namespace Sentinel { namespace Assets
{
	public ref class WMaterial
	{
		DECLARE_REF( Material );

	public:

		WMaterial();
		WMaterial( WColorRGBA^ ambient,
				   WColorRGBA^ diffuse,
				   WColorRGBA^ specular,
				   float spec_comp );
		WMaterial( Material& material );
		WMaterial( WMaterial^ material );
		
		////////////////////////////////

		DECLARE_PROPERTY( WColorRGBA^,	Ambient );
		DECLARE_PROPERTY( WColorRGBA^,	Diffuse );
		DECLARE_PROPERTY( WColorRGBA^,	Specular );
		DECLARE_PROPERTY( float,		SpecularComponent );

		////////////////////////////////

		DECLARE_OP_DEREF( Material );
	};

	DECLARE_CLASS_REF( Material );

	WStdVector_Class( Material, Material, WMaterial, RMaterial );
	RStdVector_Class( Material, Material, WMaterial, RMaterial );
}}
