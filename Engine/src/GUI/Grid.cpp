#pragma once

#include "GUI/Grid.h"

namespace Sentinel { namespace GUI
{
	Grid::Grid() :
		mCell( NULL ),
		mNumCellX( 0 ),
		mNumCellY( 0 )
	{}

	Grid::~Grid()
	{
		Release();
	}

	void Grid::Release()
	{
		if( mCell )
		{
			for( UINT x = 0; x < mNumCellX; ++x )
				delete[] mCell[ x ];

			delete[] mCell;
		}
	}

	void Grid::Update()
	{
		Widget::PreUpdate();

		Widget::PostUpdate();
	}

	void Grid::SetCellSize( UINT sizeX, UINT sizeY )
	{
		mNumCellX = sizeX;
		mNumCellY = sizeY;

		Release();

		mCell = new CellInfo*[ sizeX ];

		for( UINT x = 0; x < sizeX; ++x )
			mCell[ x ] = new CellInfo[ sizeY ];
	}

	UINT Grid::NumCellX()
	{
		return mNumCellX;
	}

	UINT Grid::NumCellY()
	{
		return mNumCellY;
	}

	Grid::CellInfo& Grid::Get( UINT cellX, UINT cellY )
	{
		_ASSERT( cellX < mNumCellX );
		_ASSERT( cellY < mNumCellY );

		return mCell[ cellX ][ cellY ];
	}
}}
