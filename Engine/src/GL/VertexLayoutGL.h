#pragma once

#include "CommonGL.h"
#include "VertexLayout.h"

namespace Sentinel
{
	class VertexLayoutGL : public VertexLayout
	{
	public:

		struct AttributeGL
		{
			GLuint	mType;			// type of value
			UINT	mOffset;		// offset value for attribute
			UINT	mCount;			// offset count of type
			bool	mNormalize;		// normalize these values?
		};

	private:

		std::vector< AttributeGL > mAttributeGL;

	public:

		VertexLayoutGL();
		~VertexLayoutGL();

		const AttributeGL& GetAttribute( UINT index ) const;

	private:

		void SetAttribute( UINT offset, GLuint type, bool normalize = false );

	public:

		void AddAttribute( AttributeType type );
	};
}
