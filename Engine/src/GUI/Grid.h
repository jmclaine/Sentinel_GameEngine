#pragma once

#include "GUI/Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL Grid : public Widget
	{
	protected:

		struct CellInfo
		{
			UINT    mRatioX;
			UINT    mRatioY;

			Widget* mWidget;
		};

		UINT mNumCellX;
		UINT mNumCellY;

	public:

		Grid();
		virtual ~Grid();

		virtual void Update();
	};
}}
