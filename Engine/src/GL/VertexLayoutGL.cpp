#include "VertexLayoutGL.h"

namespace Sentinel
{
	VertexLayoutGL::VertexLayoutGL()
	{}

	VertexLayoutGL::~VertexLayoutGL()
	{}

	//////////////////////////////////////

	const VertexLayoutGL::AttributeGL& VertexLayoutGL::GetAttribute( UINT index ) const
	{
		return mAttributeGL[ index ];
	}

	//////////////////////////////////////

	void VertexLayoutGL::SetAttribute( UINT offset, GLuint type, bool normalize )
	{
		mAttributeGL.push_back( AttributeGL() );

		AttributeGL& attr = mAttributeGL.back();

		attr.mType      = type;
		attr.mOffset    = mVertexSize;
		attr.mNormalize = normalize;

		switch( type )
		{
		case GL_FLOAT:
			attr.mCount = offset >> 2;
			break;

		case GL_UNSIGNED_BYTE:
			attr.mCount = offset;
			break;

		default:
			attr.mCount = offset >> 2;	// assume float or int
			break;
		}

		mVertexSize += offset;
	}

	//////////////////////////////////////

	void VertexLayoutGL::AddAttribute( VertexAttribute::Type type )
	{
		switch( type )
		{
		case VertexAttribute::TEXCOORD0:
		case VertexAttribute::TEXCOORD1:
		case VertexAttribute::TEXCOORD2:
			SetAttribute( 8, GL_FLOAT );
			break;

		case VertexAttribute::POSITION:
		case VertexAttribute::NORMAL:
			SetAttribute( 12, GL_FLOAT );
			break;

		case VertexAttribute::QUADCOORD0:
		case VertexAttribute::QUADCOORD1:
		case VertexAttribute::QUADCOORD2:
		case VertexAttribute::TANGENT:
		case VertexAttribute::BONE_WEIGHT:
			SetAttribute( 16, GL_FLOAT );
			break;

		case VertexAttribute::COLOR:
			SetAttribute( 4, GL_UNSIGNED_BYTE, true );
			break;

		case VertexAttribute::BONE_COUNT:
			SetAttribute( 4, GL_UNSIGNED_BYTE );
			break;

		case VertexAttribute::BONE_INDEX:
			SetAttribute( 16, GL_UNSIGNED_BYTE );
			break;

		case VertexAttribute::MATRIX:
			SetAttribute( 16, GL_FLOAT );
			SetAttribute( 16, GL_FLOAT );
			SetAttribute( 16, GL_FLOAT );
			SetAttribute( 16, GL_FLOAT );
			break;

		default:
			REPORT_ERROR( "Failed to create invalid type Attribute!", "Shader Error" );
		}

		mLayout.push_back( type );
	}
}
