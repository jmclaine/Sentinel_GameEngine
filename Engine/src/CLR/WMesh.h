#pragma once

#include "m_shared_ptr.h"
#include "Property.h"
#include "Mesh.h"
#include "WRendererTypes.h"

namespace Sentinel { namespace Wrapped
{
	ref class WRenderer;
	ref class WGameWorld;
	ref class WBuffer;
	ref class WShader;
	ref class RTexture;
	ref class WMaterial;
	ref class WMatrix4f;
	ref class WVector4f;

	public ref class WMesh
	{
		DECLARE_REF_SHARED( Mesh );

	public:

		DECLARE_OP_SHARED( Mesh );

		static bool operator == ( WMesh^ mesh0, WMesh^ mesh1 );

		DECLARE_PROPERTY( Sentinel::Wrapped::PrimitiveType,	Primitive );
		
		DECLARE_PROPERTY( WBuffer^,			VBO );
		DECLARE_PROPERTY( WBuffer^,			IBO );

		DECLARE_PROPERTY( WMatrix4f^,		MatrixWorld );
		DECLARE_PROPERTY( WMatrix4f^,		MatrixShadow );
		DECLARE_PROPERTY( WVector4f^,		TextureScale );
		
		DECLARE_PROPERTY( WShader^,			Shader );
		DECLARE_PROPERTY( WMaterial^,		Material );

		RTexture^	Texture( TextureType type );

		void		Draw( WRenderer^ renderer, WGameWorld^ world );
		void		Draw( WRenderer^ renderer, WGameWorld^ world, UINT count );
	};

	DECLARE_CLASS_REF_SHARED( Mesh );
}}
