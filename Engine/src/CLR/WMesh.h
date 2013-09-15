#pragma once

#include "m_shared_ptr.h"
#include "Property.h"
#include "Mesh.h"
#include "WRenderer.h"
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

		static bool operator == ( WMesh^ mesh0, WMesh^ mesh1 );

		DECLARE_PROPERTY( Sentinel::Systems::PrimitiveType,	Primitive );
		
		DECLARE_PROPERTY( WBuffer^,			VBO );
		DECLARE_PROPERTY( WBuffer^,			IBO );

		DECLARE_PROPERTY( WMatrix4f^,		MatrixWorld );
		DECLARE_PROPERTY( WMatrix4f^,		MatrixShadow );
		DECLARE_PROPERTY( WVector4f^,		TextureScale );
		
		DECLARE_PROPERTY( WShader^,			Shader );
		DECLARE_PROPERTY( WMaterial^,		Material );

		RTexture^	Texture( TextureType type );

		void		Draw();
		void		Draw( UINT count );
	};
}}
