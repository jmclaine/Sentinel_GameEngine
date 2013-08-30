#pragma once

#include "m_shared_ptr.h"
#include "Mesh.h"
#include "WColor.h"
#include "WMaterial.h"
#include "WShader.h"
#include "WTexture.h"
#include "RBuffer.h"
#include "WMatrix4f.h"
#include "WVector4f.h"

using namespace Sentinel::Math;

namespace Sentinel { namespace Assets
{
	public enum class TextureType
	{
		TEXTURE_DIFFUSE,
		TEXTURE_NORMAL,
		TEXTURE_DEPTH,

		NUM_TEXTURES,
	};

	public ref class RMesh
	{
	private:

		m_shared_ptr< Mesh >	mRef;

	public:

		RMesh( Mesh* mesh );
		RMesh( std::shared_ptr< Mesh > mesh );
		~RMesh();

		std::shared_ptr< Mesh >	GetRef();

		////////////////////////////////

		void		SetWorldTransform( WMatrix4f^ world );
		void		SetShadowTransform( WMatrix4f^ shadow );
		void		SetTextureScale( WVector4f^ scale );

		void		SetShader( WShader^ shader );
		void		SetMaterial( WMaterial^ material );
		void		SetTexture( WTexture^ texture, TextureType type );

		RBuffer^	GetVBO();
		RBuffer^	GetIBO();

		void		Draw();
		void		Draw( UINT count );
	};
}}
