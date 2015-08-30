#pragma once

#include <vector>

#include "Sentinel.h"
#include "Types.h"
#include "RendererTypes.h"

namespace Sentinel
{
	class SENTINEL_DLL VertexLayout
	{
	protected:

		std::vector<VertexAttribute::Type> mLayout;

		UINT mVertexSize;

		///////////////////////////////////

		VertexLayout();

	public:

		virtual ~VertexLayout();

		///////////////////////////////////

		const std::vector<VertexAttribute::Type>& Layout() const;

		UINT VertexSize() const;
	};
}
