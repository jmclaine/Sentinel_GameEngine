#include "GUI/WidgetWorld.h"
#include "GUI/WidgetObject.h"
#include "SpriteSystem.h"
#include "FontSystem.h"
#include "Renderer.h"
#include "GameWorld.h"
#include "Serializable.h"

namespace Sentinel { namespace GUI
{
	WidgetWorld::WidgetWorld() :
		mWindowInfo( NULL ),
		mGameWorld( NULL ),
		mSpriteSystem( NULL ),
		mFontSystem( NULL )
	{}

	WidgetWorld::~WidgetWorld()
	{
		Release();
	}

	void WidgetWorld::Release()
	{
		TRAVERSE_VECTOR( x, mWidgetObject )
		{
			delete mWidgetObject[ x ];
		}
	}

	// All associated WidgetObjects and Widgets also Startup().
	//
	void WidgetWorld::Startup()
	{
		TRAVERSE_VECTOR( x, mWidgetObject )
		{
			mWidgetObject[ x ]->Startup();
		}
	}

	// All associated WidgetObjects and Widgets also Update().
	//
	void WidgetWorld::Update()
	{
		TRAVERSE_VECTOR( x, mWidgetObject )
		{
			mWidgetObject[ x ]->UpdateController();
		}

		TRAVERSE_VECTOR( x, mWidgetObject )
		{
			mWidgetObject[ x ]->UpdateModel();
		}

		TRAVERSE_VECTOR( x, mWidgetObject )
		{
			mWidgetObject[ x ]->UpdateView();
		}
	}

	// All associated WidgetObjects and Widgets also Shutdown().
	//
	void WidgetWorld::Shutdown()
	{
		TRAVERSE_VECTOR( x, mWidgetObject )
		{
			mWidgetObject[ x ]->Shutdown();
		}
	}

	// Returns entity.
	//
	WidgetObject* WidgetWorld::AddWidgetObject( WidgetObject* entity )
	{
		if( entity )
		{
			TRAVERSE_VECTOR( x, mWidgetObject )
			{
				if( mWidgetObject[ x ] == entity )
					return entity;
			}

			if( entity->GetParent() )
				entity->GetParent()->RemoveChild( entity );

			entity->SetWorld( this );
		
			mWidgetObject.push_back( entity );
		}

		return entity;
	}

	// Removes and returns entity.
	// Entity must be deleted unless attached to another WidgetObject.
	//
	WidgetObject* WidgetWorld::RemoveWidgetObject( WidgetObject* entity )
	{
		TRAVERSE_LIST( it, mWidgetObject )
		{
			if( (*it) == entity )
			{
				mWidgetObject.erase( it );

				entity->SetWorld( NULL );

				return entity;
			}
		}
		
		return entity;
	}

	// Returns WidgetObject at index.
	//
	WidgetObject* WidgetWorld::GetWidgetObject( UINT index )
	{
		_ASSERT( index < mWidgetObject.size() );

		return mWidgetObject[ index ];
	}

	UINT WidgetWorld::NumWidgetObjects()
	{
		return mWidgetObject.size();
	}

	//////////////////////////////////////////////////////////////////

	void WidgetWorld::Save( Archive& archive )
	{
		UINT size = mWidgetObject.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mWidgetObject )
		{
			mWidgetObject[ x ]->Save( archive );
		}
	}

	void WidgetWorld::Load( Archive& archive )
	{
		Release();

		UINT size = 0;
		archive.Read( &size );

		WidgetObject* obj;
		for( UINT x = 0; x < size; ++x )
		{
			obj = (WidgetObject*)SerialRegister::Load( archive );
			AddWidgetObject( obj );
			obj->Load( archive );
		}
	}
}}
