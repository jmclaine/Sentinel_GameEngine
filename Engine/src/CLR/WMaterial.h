#pragma once

#include "Material.h"
#include "WColor.h"
#include "WFloat.h"

namespace Sentinel { namespace Assets
{
	public ref class WMaterial
	{
	private:

		Material*		mRef;

	public:

		WMaterial();
		WMaterial( WColorRGBA^ ambient,
				   WColorRGBA^ diffuse,
				   WColorRGBA^ specular,
				   float spec_comp );
		WMaterial( Material& material );
		WMaterial( WMaterial^ material );
		WMaterial( WMaterial% material );
		~WMaterial();
		!WMaterial();

		Material*		GetRef();

		WColorRGBA^		Ambient();
		WColorRGBA^		Diffuse();
		WColorRGBA^		Specular();
		WFloat^			SpecularComponent();
	};
}}
