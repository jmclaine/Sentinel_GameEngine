#pragma once
/*
CellRatios are UINT values so that the mathematics behind
the actual floating point values are not required calculations.
*/
#include "GUI/Widget.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL Grid : public Widget
	{
	public:

		struct CellInfo
		{
			UINT		mRatioX;
			UINT		mRatioY;

			Widget*		mWidget;

			CellInfo() :
				mRatioX( 1 ),
				mRatioY( 1 ),
				mWidget( NULL )
			{}
		};

	protected:

		CellInfo**		mCell;

		UINT			mNumCellX;
		UINT			mNumCellY;

	public:

		Grid();
		virtual ~Grid();

	private:

		void			Release();

	public:

		virtual void	Update();

		void			SetCellSize( UINT sizeX, UINT sizeY );

		UINT			NumCellX();
		UINT			NumCellY();

		CellInfo&		Get( UINT cellX, UINT cellY );
	};
}}
