#include "GUI/LabelWidget.h"
#include "GUI/WidgetObject.h"
#include "GUI/WidgetWorld.h"
#include "GUI/ModelWidget.h"
#include "FontSystem.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( LabelWidget );
	DEFINE_SERIAL_CLONE( LabelWidget );

	LabelWidget::LabelWidget()
	{}

	LabelWidget::LabelWidget( const std::string& text ) :
		mText( text ),
		mColor( 1, 1, 1, 1 )
	{}

	LabelWidget::~LabelWidget()
	{}

	//////////////////////////////////

	void LabelWidget::Startup()
	{}

	void LabelWidget::Update()
	{
		_ASSERT( mOwner->GetModel() );
		_ASSERT( mOwner->GetWorld()->mFontSystem );

		if( mIsVisible )
		{
			Matrix4f matWorld = mOwner->GetModel()->GetMatrixWorld();

			float x = matWorld[12];
			float y = matWorld[13];

			FontSystem* font = mOwner->GetWorld()->mFontSystem;

			for( UINT i = 0; i < mText.size(); ++i )
			{
				char& c = mText[ i ];

				matWorld[12] = x + font->mFont->mOffsetX[ c ];
				matWorld[13] = y - font->mFont->mOffsetY[ c ];
			
				font->Draw( c, mColor, mOwner->GetWorld()->mMatrixWVP * matWorld );

				x += font->mFont->mAdvance[ c ];
				matWorld[14] -= 0.0001f;
			}
		}
	}

	void LabelWidget::Shutdown()
	{}

	//////////////////////////////////

	DEFINE_SERIAL_REGISTER_SAVE( LabelWidget );

	void LabelWidget::Save( Archive& archive )
	{
		archive.Write( &mText );
		archive.Write( mColor.Ptr(), ar_sizeof( mColor ));
	}

	void LabelWidget::Load( Archive& archive )
	{
		archive.Read( &mText );
		archive.Read( mColor.Ptr(), ar_sizeof( mColor ));
	}
}}
