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

	void VertexLayoutGL::AddAttribute( AttributeType type )
	{
		switch( type )
		{
		case ATTRIB_TEXCOORD0:
		case ATTRIB_TEXCOORD1:
		case ATTRIB_TEXCOORD2:
			SetAttribute( 8, GL_FLOAT );
			break;

		case ATTRIB_POSITION:
		case ATTRIB_NORMAL:
			SetAttribute( 12, GL_FLOAT );
			break;

		case ATTRIB_QUADCOORD0:
		case ATTRIB_QUADCOORD1:
		case ATTRIB_QUADCOORD2:
		case ATTRIB_TANGENT:
		case ATTRIB_BONE_WEIGHT:
			SetAttribute( 16, GL_FLOAT );
			break;

		case ATTRIB_COLOR:
			SetAttribute( 4, GL_UNSIGNED_BYTE, true );
			break;

		case ATTRIB_BONE_COUNT:
			SetAttribute( 4, GL_UNSIGNED_BYTE );
			break;

		case ATTRIB_BONE_INDEX:
			SetAttribute( 16, GL_UNSIGNED_BYTE );
			break;

		case ATTRIB_MATRIX:
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
