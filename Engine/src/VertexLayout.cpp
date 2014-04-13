#include "VertexLayout.h"

namespace Sentinel
{
	VertexLayout::VertexLayout() :
		mVertexSize( 0 )
	{}

	VertexLayout::~VertexLayout()
	{}

	const std::vector< VertexAttribute::Type >& VertexLayout::Layout() const
	{
		return mLayout;
	}

	UINT VertexLayout::VertexSize() const
	{
		return mVertexSize;
	}
}
