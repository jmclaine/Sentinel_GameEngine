#include "GUI/Label.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "FontSystem.h"
#include "SpriteSystem.h"
#include "RenderManager.h"
#include "Material.h"
#include "Component/Transform.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( Label );
	DEFINE_SERIAL_CLONE( Label );

	Label::Label()
	{}

	Label::Label( const std::string& text ) :
		mText( text ),
		mColor( 1, 1, 1, 1 )
	{}

	Label::~Label()
	{}

	//////////////////////////////////

	void Label::Startup()
	{
		Component::Drawable::Startup();

		mFontSystem = mOwner->GetWorld()->mFontSystem;

		mOwner->GetWorld()->mRenderManager->Draw( this, mFontSystem->mSpriteSystem->mMaterial->mRenderQueue, mOwner->mLayer );
	}

	void Label::Update()
	{}

	void Label::Shutdown()
	{}

	//////////////////////////////////

	void Label::CalculateBounds()
	{}

	bool Label::CheckVisible( Component::Camera* camera )
	{
		return true;
	}

	void Label::Draw()
	{
		if( mEnabled )
		{
			mFontSystem->Clear();

			std::shared_ptr< Font > font = mFontSystem->mFont;

			Vector2f scale( mTransform->mScale.x / font->mSize.x, mTransform->mScale.y / font->mSize.y );
			
			Matrix4f matWVP = mOwner->GetWorld()->mCurrentCamera->GetMatrixWVP();

			Vector3f offset;

			for( UINT i = 0; i < mText.size(); ++i )
			{
				char& c = mText[ i ];

				offset.x += font->mOffsetX[ c ] * scale.x;
				offset.y = -font->mOffsetY[ c ] * scale.y;

				mFontSystem->Draw( c, mColor, matWVP * mTransform->GetMatrixWorld( offset ));

				offset.x += font->mAdvance[ c ] * scale.x;
				offset.z -= 0.0001f;
			}

			mFontSystem->Present();
		}
	}

	//////////////////////////////////

	DEFINE_SERIAL_REGISTER_SAVE( Label );

	void Label::Save( Archive& archive )
	{
		Component::Drawable::Save( archive );

		archive.Write( &mText );
		archive.Write( mColor.Ptr(), ar_sizeof( mColor ));
	}

	void Label::Load( Archive& archive )
	{
		Component::Drawable::Load( archive );

		archive.Read( &mText );
		archive.Read( mColor.Ptr(), ar_sizeof( mColor ));
	}

	//////////////////////////////////

	GameComponent* Label::Copy()
	{
		Label* label = new Label( mText );

		Component::Drawable::Copy( label );

		label->mColor = mColor;

		return label;
	}
}}
