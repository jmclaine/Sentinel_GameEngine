#pragma once

#include "m_shared_ptr.h"
#include "Texture.h"

using namespace System;

namespace Sentinel { namespace Assets
{
	public ref class WTexture
	{
	protected:

		Texture*		mRef;

		////////////////////////////////

		WTexture();

	public:

		WTexture( Texture* texture );
		~WTexture();
		!WTexture();

	protected:

		virtual void	Delete();

	public:
		
		Texture*		GetRef();

		////////////////////////////////

		void			Release();

		String^			Filename();
		UINT			Width();
		UINT			Height();
	};

	public ref class RTexture : public WTexture
	{
	private:

		Texture*&		mRefPtr;

	public:

		RTexture( Texture*& texture );
		
		void			Set( Texture*& texture );
		void			Set( WTexture^ texture );

	protected:

		virtual void	Delete() override;
	};
}}
