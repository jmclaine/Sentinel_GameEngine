#include "WGameWorld.h"
#include "WString.h"

#include <msclr\marshal_cppstd.h>

using namespace System::Runtime::InteropServices;

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	void WGameWorld::Create()
	{
		GameWorld::Inst( GameWorld::Create() );
	}

	void WGameWorld::Release()
	{
		GameWorld::Inst()->Release();
	}

	void WGameWorld::Save( WArchive^ archive )
	{
		GameWorld::Inst()->Save( *archive->GetRef() );
	}

	void WGameWorld::Load( WArchive^ archive )
	{
		GameWorld::Inst()->Load( *archive->GetRef() );
	}

	void WGameWorld::Startup()
	{
		GameWorld::Inst()->Startup();
	}

	void WGameWorld::Update()
	{
		GameWorld::Inst()->Update();
	}

	void WGameWorld::UpdateController()
	{
		GameWorld::Inst()->UpdateController();
	}

	void WGameWorld::UpdatePhysics()
	{
		GameWorld::Inst()->UpdatePhysics();
	}

	void WGameWorld::UpdateTransform()
	{
		GameWorld::Inst()->UpdateTransform();
	}

	void WGameWorld::UpdateComponents()
	{
		GameWorld::Inst()->UpdateComponents();
	}

	void WGameWorld::UpdateDrawable()
	{
		GameWorld::Inst()->UpdateDrawable();
	}

	void WGameWorld::Shutdown()
	{
		GameWorld::Inst()->Shutdown();
	}

	//////////////////////////////

	WGameObject^ WGameWorld::AddGameObject( WGameObject^ entity )
	{
		GameWorld::Inst()->AddGameObject( entity->GetRef() );
		return entity;
	}

	WGameObject^ WGameWorld::AddGameObject( WGameObject^ entity, System::String^ name )
	{
		GameWorld::Inst()->AddGameObject( entity->GetRef(), WString::Alloc( name ));
		return entity;
	}

	WGameObject^ WGameWorld::RemoveGameObject( WGameObject^ entity )
	{
		GameWorld::Inst()->RemoveGameObject( entity->GetRef() );
		return entity;
	}

	WGameObject^ WGameWorld::GetGameObject( UINT index )
	{
		return gcnew WGameObject( GameWorld::Inst()->GetGameObject( index ));
	}

	UINT WGameWorld::NumGameObjects()
	{
		return GameWorld::Inst()->NumGameObjects();
	}

	//////////////////////////////

	void WGameWorld::SetCamera( WCameraComponent^ camera )
	{
		GameWorld::Inst()->SetCamera( (CameraComponent*)camera->GetRef() );
	}

	WCameraComponent^ WGameWorld::GetCamera( int index )
	{
		return gcnew WCameraComponent( GameWorld::Inst()->GetCamera( index ));
	}

	WLightComponent^ WGameWorld::GetLight( UINT index )
	{
		return gcnew WLightComponent( GameWorld::Inst()->GetLight( index ));
	}
}}
