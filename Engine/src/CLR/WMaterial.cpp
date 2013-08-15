#include "WMaterial.h"
#include "Color.h"
#include "Util.h"

namespace Sentinel { namespace Assets
{
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

	WMaterial::WMaterial( WMaterial% material )
	{
		mRef = new Material( *material.mRef );
	}

	WMaterial::~WMaterial()
	{
		delete mRef;
	}

	WMaterial::!WMaterial()
	{
		delete mRef;
	}

	Material* WMaterial::GetRef()
	{
		return mRef;
	}

	WColorRGBA^ WMaterial::Ambient()
	{
		return gcnew WColorRGBA( &mRef->mAmbient );
	}

	WColorRGBA^ WMaterial::Diffuse()
	{
		return gcnew WColorRGBA( &mRef->mDiffuse );
	}

	WColorRGBA^ WMaterial::Specular()
	{
		return gcnew WColorRGBA( &mRef->mSpecular );
	}

	WFloat^ WMaterial::SpecularComponent()
	{
		return gcnew WFloat( &mRef->mSpecularComponent );
	}
}}
