#pragma once

#include "m_shared_ptr.h"
#include "Mesh.h"
#include "WColor.h"
#include "WMaterial.h"
#include "WShader.h"
#include "WTexture.h"
#include "WBuffer.h"
#include "WMatrix4f.h"
#include "WVector4f.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Assets
{
	public enum class TextureType
	{
		DIFFUSE,
		NORMAL,
		DEPTH,

		NUM_TEXTURES,
	};

	public ref class WMesh
	{
		DECLARE_REF_SHARED( Mesh );

	public:

		void		SetWorldTransform( WMatrix4f^ world );
		void		SetShadowTransform( WMatrix4f^ shadow );
		void		SetTextureScale( WVector4f^ scale );

		void		SetShader( WShader^ shader );
		void		SetMaterial( WMaterial^ material );
		void		SetTexture( WTexture^ texture, TextureType type );

		WBuffer^	GetVBO();
		WBuffer^	GetIBO();

		void		Draw();
		void		Draw( UINT count );
	};
}}
