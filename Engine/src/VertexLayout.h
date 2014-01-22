#pragma once

#include <vector>

#include "Common.h"

namespace Sentinel
{
	enum AttributeType
	{
		ATTRIB_POSITION,
		ATTRIB_TEXCOORD0,
		ATTRIB_TEXCOORD1,
		ATTRIB_TEXCOORD2,
		ATTRIB_QUADCOORD0,
		ATTRIB_QUADCOORD1,
		ATTRIB_QUADCOORD2,
		ATTRIB_NORMAL,
		ATTRIB_COLOR,
		ATTRIB_TANGENT,
		ATTRIB_BONE_COUNT,
		ATTRIB_BONE_INDEX,
		ATTRIB_BONE_WEIGHT,
		ATTRIB_MATRIX,

		NUM_ATTRIBUTES
	};

	class SENTINEL_DLL VertexLayout
	{
	protected:

		std::vector< AttributeType > mLayout;

		UINT	mVertexSize;

		///////////////////////////////////

		VertexLayout();

	public:

		virtual ~VertexLayout();

		///////////////////////////////////

		const std::vector< AttributeType >& Layout() const;

		UINT			VertexSize() const;
	};
}
