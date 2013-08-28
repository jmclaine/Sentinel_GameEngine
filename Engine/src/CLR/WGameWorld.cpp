#include "WGameWorld.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	void WGameWorld::Load( System::String^ mapName )
	{
		GameWorld::Inst( GameWorld::Load( RString::ToString( mapName )));
	}

	void WGameWorld::Load( System::String^ mapName, IntPtr hWnd )
	{
		GameWorld::Inst( GameWorld::Load( RString::ToString( mapName ), hWnd.ToPointer() ));
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

	void WGameWorld::UpdateDrawable()
	{
		GameWorld::Inst()->UpdateDrawable();
	}

	void WGameWorld::UpdateComponents()
	{
		GameWorld::Inst()->UpdateComponents();
	}

	void WGameWorld::Shutdown()
	{
		GameWorld::Inst()->Shutdown();
	}

	//////////////////////////////

	RGameObject^ WGameWorld::AddGameObject( WGameObject^ entity, System::String^ name )
	{
		return gcnew RGameObject( GameWorld::Inst()->AddGameObject( entity->GetRef(), RString::ToString( name )));
	}

	RGameObject^ WGameWorld::RemoveGameObject( RGameObject^ entity )
	{
		GameWorld::Inst()->RemoveGameObject( entity->GetRef() );
		return entity;
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

	RGameObject^ WGameWorld::GetGameObject( UINT index )
	{
		return gcnew RGameObject( GameWorld::Inst()->GetGameObject( index ));
	}
}}
