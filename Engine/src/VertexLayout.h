#pragma once

#include <vector>

#include "Sentinel.h"

namespace Sentinel
{
	enum class VertexAttribute : BYTE
	{
		POSITION,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		QUADCOORD0,
		QUADCOORD1,
		QUADCOORD2,
		NORMAL,
		COLOR,
		TANGENT,
		BONE_COUNT,
		BONE_INDEX,
		BONE_WEIGHT,
		MATRIX,

		COUNT
	};

	class SENTINEL_DLL VertexLayout
	{
	protected:
		std::vector<VertexAttribute> mLayout;

		UINT mVertexSize;

		///////////////////////////////////

		VertexLayout();

	public:
		virtual ~VertexLayout();

		///////////////////////////////////

		const std::vector<VertexAttribute>& Layout() const;

		UINT VertexSize() const;
	};
}
