#include <algorithm>

#include "Util.h"
#include "Timing.h"
#include "GameObject.h"
#include "GameWorld.h"

namespace Sentinel
{
	DEFINE_SERIAL_OBJ( GameObject );

	GameObject::GameObject() :
		mTransform( NULL ), mController( NULL ), mPhysics( NULL ), mDrawable( NULL ), 
		mParent( NULL ), mName( NULL )
	{}

	GameObject::~GameObject()
	{
		Shutdown();
	}

	//////////////////////////////

	DEFINE_SERIAL_CLONE( GameObject );

	void GameObject::Serialize( FILE* file )
	{}

	void GameObject::Deserialize( FILE* file )
	{}

	//////////////////////////////

	GameComponent* GameObject::AttachComponent( GameComponent* component, const char* name )
	{
		component->SetOwner( this );
		component->mName = name;
		
		switch( component->GetType() )
		{
			case GameComponent::TRANSFORM:
				_ASSERT( !mTransform );

				mTransform = component;
				return component;

			case GameComponent::CONTROLLER:
				_ASSERT( !mController );

				mController = component;
				return component;

			case GameComponent::PHYSICS:
				_ASSERT( !mPhysics );
				
				mPhysics = component;
				return component;

			case GameComponent::DRAWABLE:
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

		switch( component->GetType() )
		{
			case GameComponent::TRANSFORM:
				DETACH_COMPONENT( mTransform );

			case GameComponent::CONTROLLER:
				DETACH_COMPONENT( mController );

			case GameComponent::PHYSICS:
				DETACH_COMPONENT( mPhysics );

			case GameComponent::DRAWABLE:
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

	void GameObject::AddChild( GameObject* obj )
	{
		// Check if the object is already a child.
		//
		TRAVERSE_VECTOR( x, mChild )
			if( mChild[ x ] == obj )
				return;

		// Remove the object from its parent.
		//
		if( obj->mParent )
			obj->mParent->RemoveChild( obj );

		// Add the object as a child.
		//
		mChild.push_back( obj );
		obj->mParent = this;

		// Ensure the GameWorld no longer has
		// the object as a parent object.
		//
		GameWorld::Inst()->RemoveGameObject( obj );
	}

	void GameObject::RemoveChild( GameObject* obj )
	{
		TRAVERSE_LIST( it, mChild )
			if( *it == obj )
			{
				obj->mParent = NULL;
				mChild.erase( it );
				return;
			}
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
			if( mComponent[ x ]->GetType() != GameComponent::CAMERA && 
				mComponent[ x ]->GetType() != GameComponent::LIGHT )
				mComponent[ x ]->Update();
	}

	void GameObject::UpdateDrawable( bool drawChildren )
	{
		if( mDrawable )
			mDrawable->Update();
		
		if( drawChildren )
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

	GameComponent* GameObject::FindComponent( GameComponent::Type type )
	{
		switch( type )
		{
			case GameComponent::TRANSFORM:
				return mTransform;
				
			case GameComponent::CONTROLLER:
				return mController;

			case GameComponent::PHYSICS:
				return mPhysics;
				
			case GameComponent::DRAWABLE:
				return mDrawable;
				
			default:
				TRAVERSE_VECTOR( x, mComponent )
				{
					if( mComponent[ x ]->GetType() == type )
						return mComponent[ x ];
				}
				break;
		}

		return NULL;
	}
}
