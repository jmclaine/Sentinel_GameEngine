#include "Util.h"
#include "Timing.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "Component/Drawable.h"
#include "Component/Camera.h"

namespace Sentinel
{
	DEFINE_SERIAL_REGISTER( GameObject );

	GameObject::GameObject( const std::string& name ) :
		mTransform( NULL ),
		mController( NULL ),
		mPhysics( NULL ),
		mDrawable( NULL ),
		mCamera( NULL ),
		mWorld( NULL ),
		mName( name ),
		mLayer( 0x0001 ),
		mEnabled( true )
	{}

	GameObject::~GameObject()
	{
		SAFE_DELETE( mTransform );
		SAFE_DELETE( mController );
		SAFE_DELETE( mPhysics );
		SAFE_DELETE( mDrawable );
		SAFE_DELETE( mCamera );

		TRAVERSE_VECTOR( x, mComponent )
			delete mComponent[ x ];
	}

	DEFINE_SERIAL_CLONE( GameObject );

	////////////////////////////////////////////////////////////

	GameComponent* GameObject::AttachComponent( GameComponent* component )
	{
		if( !component )
			return NULL;

		component->SetOwner( this );
		
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

			case GameComponent::CAMERA:
				_ASSERT( !mCamera );

				mCamera = component;
				return component;

			default:
				mComponent.push_back( component );
				return component;
		}
	}

#define DETACH_COMPONENT( obj )\
	if( component == obj )\
		obj = NULL;\
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

			case GameComponent::CAMERA:
				DETACH_COMPONENT( mCamera );

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

	////////////////////////////////////////////////////////////

	GameObject* GameObject::AddChild( GameObject* obj )
	{
		if( mWorld )
		{
			mWorld->RemoveGameObject( obj );

			obj->SetWorld( mWorld );
			obj->SetParent( this );

			////////////////////////////////

			Component::Light* light = (Component::Light*)obj->FindComponent( GameComponent::LIGHT );
			
			if( light )
				mWorld->AddLight( light );
		}

		return ListNode< GameObject >::AddChild( obj );
	}

	////////////////////////////////////////////////////////////

	GameWorld* GameObject::GetWorld()
	{
		return mWorld;
	}

	void GameObject::SetWorld( GameWorld* world )
	{
		mWorld = world;

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->SetWorld( world );
		}
	}

	void GameObject::SetActive( bool active )
	{
		mEnabled = active;

		if( mDrawable && !active )
		{
			((Component::Drawable*)mDrawable)->mEnabled = false;
		}

		TRAVERSE_VECTOR( x, mChild )
		{
			mChild[ x ]->SetActive( active );
		}
	}

	////////////////////////////////////////////////////////////

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

		if( mCamera )
		{
			mWorld->AddCamera( (Component::Camera*)mCamera );
			mCamera->Startup();
		}
		
		TRAVERSE_VECTOR( x, mComponent )
		{
			if( mComponent[ x ]->GetType() == GameComponent::LIGHT )
				mWorld->AddLight( (Component::Light*)mComponent[ x ] );

			mComponent[ x ]->Startup();
		}

		TRAVERSE_VECTOR( x, mChild )
			mChild[ x ]->Startup();
	}

	void GameObject::UpdateController()
	{
		if( mEnabled )
		{
			if( mController )
				mController->Update();

			TRAVERSE_VECTOR( x, mChild )
				mChild[ x ]->UpdateController();
		}
	}

	void GameObject::UpdatePhysics()
	{
		if( mEnabled )
		{
			if( mPhysics )
				mPhysics->Update();

			TRAVERSE_VECTOR( x, mChild )
				mChild[ x ]->UpdatePhysics();
		}
	}

	void GameObject::UpdateTransform()
	{
		if( mEnabled )
		{
			if( mTransform )
				mTransform->Update();

			TRAVERSE_VECTOR( x, mChild )
				mChild[ x ]->UpdateTransform();
		}
	}

	void GameObject::UpdateComponents()
	{
		if( mEnabled )
		{
			// WARNING
			TRAVERSE_VECTOR( x, mComponent )
			{
				if( mComponent[ x ]->GetType() != GameComponent::LIGHT )
					mComponent[ x ]->Update();
			}

			TRAVERSE_VECTOR( x, mChild )
				mChild[ x ]->UpdateComponents();
		}
	}

	void GameObject::UpdateCamera()
	{
		if( mEnabled )
		{
			if( mCamera )
				mCamera->Update();
		
			TRAVERSE_VECTOR( x, mChild )
				mChild[ x ]->UpdateCamera();
		}
	}

	void GameObject::UpdateDrawable()
	{
		if( mEnabled )
		{
			if( mDrawable )
				mDrawable->Update();
		
			TRAVERSE_VECTOR( x, mChild )
				mChild[ x ]->UpdateDrawable();
		}
	}

#define SHUTDOWN_COMPONENT( component )\
	if( component ) { component->Shutdown(); }

	void GameObject::Shutdown()
	{
		SHUTDOWN_COMPONENT( mTransform );
		SHUTDOWN_COMPONENT( mController );
		SHUTDOWN_COMPONENT( mPhysics );
		SHUTDOWN_COMPONENT( mDrawable );

		TRAVERSE_VECTOR( x, mComponent )
			mComponent[ x ]->Shutdown();

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

	////////////////////////////////////////////////////////////

	#define SAVE_COMPONENT( component )\
	(component) ? component->Save( archive ) : archive.Write( &noValue, 1, true );

	void GameObject::Save( Archive& archive )
	{
		SERIAL_REGISTER.Save( archive );
		
		archive.Write( &mName );
		
		int noValue = 0;
		SAVE_COMPONENT( mTransform );
		SAVE_COMPONENT( mController );
		SAVE_COMPONENT( mPhysics );
		SAVE_COMPONENT( mDrawable );

		BYTE size = (BYTE)mComponent.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mComponent )
			mComponent[ x ]->Save( archive );
		
		size = (BYTE)mChild.size();
		archive.Write( &size );

		TRAVERSE_VECTOR( x, mChild )
			mChild[ x ]->Save( archive );
	}

#define LOAD_COMPONENT()\
{	component = (GameComponent*)SerialRegister::Load( archive );\
	if( component ) {\
	AttachComponent( component );\
	component->Load( archive ); }}

	void GameObject::Load( Archive& archive )
	{
		archive.Read( &mName );

		int id = 0;
		GameComponent* component;

		LOAD_COMPONENT();
		LOAD_COMPONENT();
		LOAD_COMPONENT();
		LOAD_COMPONENT();

		BYTE size = 0;
		archive.Read( &size );

		for( BYTE x = 0; x < size; ++x )
			LOAD_COMPONENT();

		archive.Read( &size );

		GameObject* obj;
		for( BYTE x = 0; x < size; ++x )
		{
			obj = (GameObject*)SerialRegister::Load( archive );
			if( obj )
			{
				obj->SetWorld( mWorld );
				obj->Load( archive );
				AddChild( obj );
			}
		}
	}

	GameObject* GameObject::Copy()
	{
		GameObject* obj = new GameObject( mName );
		
		if( mTransform )
			obj->AttachComponent( mTransform->Copy() );
		
		if( mController )
			obj->AttachComponent( mController->Copy() );

		if( mPhysics )
			obj->AttachComponent( mPhysics->Copy() );

		if( mDrawable )
			obj->AttachComponent( mDrawable->Copy() );

		if( mCamera )
			obj->AttachComponent( mCamera->Copy() );
		
		TRAVERSE_VECTOR( x, mComponent )
		{
			obj->AttachComponent( mComponent[ x ]->Copy() );
		}

		TRAVERSE_VECTOR( x, mChild )
		{
			obj->AddChild( mChild[ x ]->Copy() );
		}

		return obj;
	}
}
