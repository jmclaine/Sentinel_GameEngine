#pragma once

#include "GUI/Label.h"

namespace Sentinel { namespace GUI
{
	Label::Label( const std::string& text ) :
		mText( text )
	{}

	Label::~Label()
	{}

	void Label::Update()
	{
		float x = mMatrixWorld[12];
		float y = mMatrixWorld[13];

		for( UINT i = 0; i < mText.size(); ++i )
		{
			char& c = mText[ i ];

			mMatrixWorld[12] = x + FONT_SYSTEM->mFont->mOffsetX[ c ];
			mMatrixWorld[13] = y - FONT_SYSTEM->mFont->mOffsetY[ c ];
			
			FONT_SYSTEM->Draw( c, mColor, MATRIX_WVP * mMatrixWorld );

			x += FONT_SYSTEM->mFont->mAdvance[ c ];
			mMatrixWorld[14] -= 0.0001f;
		}
	}
}}
