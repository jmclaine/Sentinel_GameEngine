#pragma once

#include "Material.h"
#include "WColor.h"
#include "RFloat.h"

namespace Sentinel { namespace Assets
{
	public ref class WMaterial
	{
	protected:

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

	protected:

		virtual void Delete();

	public:

		Material*	GetRef();

		////////////////////////////////

		RColorRGBA^	Ambient();
		RColorRGBA^	Diffuse();
		RColorRGBA^	Specular();
		RFloat^		SpecularComponent();
	};

	public ref class RMaterial sealed : public WMaterial
	{
	public:

		RMaterial( Material* material );
		RMaterial( WMaterial^ material );

		void		Set( const Material& material );
		void		Set( WMaterial^ material );

	protected:

		virtual void Delete() override;
	};
}}
