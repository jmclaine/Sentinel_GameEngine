#include <algorithm>

#include "Util.h"
#include "Timing.h"
#include "GameObject.h"
#include "GameWorld.h"

namespace Sentinel
{
	GameObject::GameObject() :
		mTransform( NULL ), mController( NULL ), mPhysics( NULL ), mDrawable( NULL ),
		mParent( NULL )
	{}

	GameObject::~GameObject()
	{
		Shutdown();
	}

	GameComponent* GameObject::AttachComponent( GameComponent* component, const std::string& name )
	{
		component->SetOwner( this );
		component->mName = name;
		
		switch( component->Type() )
		{
			case COMPONENT_TRANSFORM:
				_ASSERT( !mTransform );

				mTransform = component;
				return component;

			case COMPONENT_CONTROLLER:
				_ASSERT( !mController );

				mController = component;
				return component;

			case COMPONENT_PHYSICS:
				_ASSERT( !mPhysics );
				
				mPhysics = component;
				return component;

			case COMPONENT_DRAWABLE:
				_ASSERT( !mDrawable );

				mDrawable = component;
				return component;

			default:
				mComponent.push_back( component );
				return component;
		}
	}

#define DETACH_COMPONENT( obj )\
	if( component == obj )\
	{\
		obj = NULL;\
	}\
	return component;

	GameComponent* GameObject::DetachComponent( GameComponent* component )
	{
		_ASSERT( component );

		switch( component->Type() )
		{
			case COMPONENT_TRANSFORM:
				DETACH_COMPONENT( mTransform );

			case COMPONENT_CONTROLLER:
				DETACH_COMPONENT( mController );

			case COMPONENT_PHYSICS:
				DETACH_COMPONENT( mPhysics );

			case COMPONENT_DRAWABLE:
				DETACH_COMPONENT( mDrawable );

			default:
				TRAVERSE_VECTOR( x, mComponent )
				{
					if( component == mComponent[ x ] )
					{
						mComponent.erase( mComponent.begin() + x );
						return component;
					}
				}
		}

		return component;
	}

	//////////////////////////////

	GameObject* GameObject::GetParent()
	{
		return mParent;
	}

	void GameObject::AddChild( GameObject* obj )
	{
		mChild.push_back( obj );
		obj->mParent = this;

		GameWorld::Inst()->RemoveGameObject( obj );
	}

	void GameObject::RemoveChild( UINT index )
	{
		_ASSERT( index < NumChildren() );

		mChild[ index ]->mParent = NULL;
		mChild.erase( mChild.begin() + index );
	}

	GameObject* GameObject::GetChild( UINT index )
	{
		_ASSERT( index < NumChildren() );

		return mChild[ index ];
	}

	UINT GameObject::NumChildren()
	{
		return mChild.size();
	}

	//////////////////////////////

	void GameObject::Startup()
	{
		if( mTransform )
			mTransform->Startup();

		if( mController )
			mController->Startup();

		if( mPhysics )
			mPhysics->Startup();

		if( mDrawable )
			mDrawable->Startup();
		
		TRAVERSE_VECTOR( x, mComponent )
			mComponent[ x ]->Startup();

		TRAVERSE_VECTOR( x, mChild )
			mChild[ x ]->Startup();
	}

	void GameObject::UpdateController()
	{
		if( mController )
			mController->Update();

		TRAVERSE_VECTOR( x, mChild )
			mChild[ x ]->UpdateController();
	}

	void GameObject::UpdatePhysics()
	{
		if( mPhysics )
			mPhysics->Update();

		TRAVERSE_VECTOR( x, mChild )
			mChild[ x ]->UpdatePhysics();
	}

	void GameObject::UpdateTransform()
	{
		if( mTransform )
			mTransform->Update();

		TRAVERSE_VECTOR( x, mChild )
			mChild[ x ]->UpdateTransform();
	}

	void GameObject::UpdateComponents()
	{
		TRAVERSE_VECTOR( x, mComponent )
			if( mComponent[ x ]->Type() != COMPONENT_CAMERA && 
				mComponent[ x ]->Type() != COMPONENT_LIGHT )
				mComponent[ x ]->Update();
	}

	void GameObject::UpdateDrawable()
	{
		if( mDrawable )
			mDrawable->Update();

		TRAVERSE_VECTOR( x, mChild )
			mChild[ x ]->UpdateDrawable();
	}

#define SHUTDOWN_COMPONENT( obj )\
	if( obj )\
	{\
		obj->Shutdown();\
		delete obj;\
		obj = NULL;\
	}

	void GameObject::Shutdown()
	{
		SHUTDOWN_COMPONENT( mTransform );
		SHUTDOWN_COMPONENT( mController );
		SHUTDOWN_COMPONENT( mPhysics );
		SHUTDOWN_COMPONENT( mDrawable );

		TRAVERSE_VECTOR( x, mComponent )
		{
			mComponent[ x ]->Shutdown();
			delete mComponent[ x ];
		}
		mComponent.clear();

		TRAVERSE_VECTOR( x, mChild )
			mChild[ x ]->Shutdown();
	}

	GameComponent* GameObject::FindComponent( ComponentType type )
	{
		switch( type )
		{
			case COMPONENT_TRANSFORM:
				return mTransform;
				
			case COMPONENT_CONTROLLER:
				return mController;

			case COMPONENT_PHYSICS:
				return mPhysics;
				
			case COMPONENT_DRAWABLE:
				return mDrawable;
				
			default:
				TRAVERSE_VECTOR( x, mComponent )
				{
					if( mComponent[ x ]->Type() == type )
						return mComponent[ x ];
				}
				break;
		}

		return NULL;
	}
}
