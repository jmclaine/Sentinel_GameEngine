#include "WGameWorld.h"
#include "WString.h"
#include "WRenderer.h"
#include "WPhysicsSystem.h"
#include "WTiming.h"
#include "WTextureManager.h"
#include "WShaderManager.h"
#include "WMeshManager.h"
#include "WModelManager.h"
#include "WArchive.h"
#include "WGameObject.h"
#include "WCameraComponent.h"
#include "WLightComponent.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_PTR( GameWorld );

	WGameWorld::WGameWorld()
	{
		mRef = new GameWorld();
	}

	void WGameWorld::Release()
	{
		delete mRef;
	}

	DEFINE_OP_PTR( GameWorld );

	DEFINE_PROPERTY_PS_EX( GameWorld, Renderer,			XRenderer,			Renderer );
	DEFINE_PROPERTY_PS_EX( GameWorld, PhysicsSystem,	XPhysicsSystem,		PhysicsSystem );
	DEFINE_PROPERTY_PS_EX( GameWorld, Timing,			XTiming,			Timing );
	DEFINE_PROPERTY_PS_EX( GameWorld, TextureManager,	XTextureManager,	TextureManager );
	DEFINE_PROPERTY_PS_EX( GameWorld, ShaderManager,	XShaderManager,		ShaderManager );
	DEFINE_PROPERTY_PS_EX( GameWorld, MeshManager,		XMeshManager,		MeshManager );
	DEFINE_PROPERTY_PS_EX( GameWorld, ModelManager,		XModelManager,		ModelManager );
	
	void WGameWorld::Save( WArchive^ archive )
	{
		mRef->Save( *archive->GetRef() );
	}

	void WGameWorld::Load( WArchive^ archive )
	{
		mRef->Load( *archive->GetRef() );
	}

	void WGameWorld::Startup()
	{
		mRef->Startup();
	}

	void WGameWorld::UpdateController()
	{
		mRef->UpdateController();
	}

	void WGameWorld::UpdatePhysics()
	{
		mRef->UpdatePhysics();
	}

	void WGameWorld::UpdateTransform()
	{
		mRef->UpdateTransform();
	}

	void WGameWorld::UpdateComponents()
	{
		mRef->UpdateComponents();
	}

	void WGameWorld::UpdateDrawable()
	{
		mRef->UpdateDrawable();
	}

	void WGameWorld::Shutdown()
	{
		mRef->Shutdown();
	}

	//////////////////////////////

	WGameObject^ WGameWorld::AddGameObject( WGameObject^ entity )
	{
		mRef->AddGameObject( entity->GetRef() );
		return entity;
	}

	WGameObject^ WGameWorld::AddGameObject( WGameObject^ entity, System::String^ name )
	{
		mRef->AddGameObject( entity->GetRef(), WString::Alloc( name ));
		return entity;
	}

	WGameObject^ WGameWorld::RemoveGameObject( WGameObject^ entity )
	{
		mRef->RemoveGameObject( entity->GetRef() );
		return entity;
	}

	WGameObject^ WGameWorld::GetGameObject( UINT index )
	{
		return gcnew WGameObject( mRef->GetGameObject( index ));
	}

	UINT WGameWorld::NumGameObjects()
	{
		return mRef->NumGameObjects();
	}

	//////////////////////////////

	void WGameWorld::SetCamera( WCameraComponent^ camera )
	{
		mRef->SetCamera( (CameraComponent*)camera->GetRef() );
	}

	WCameraComponent^ WGameWorld::GetCamera( int index )
	{
		return gcnew WCameraComponent( mRef->GetCamera( index ));
	}

	WLightComponent^ WGameWorld::GetLight( UINT index )
	{
		return gcnew WLightComponent( mRef->GetLight( index ));
	}
}}
