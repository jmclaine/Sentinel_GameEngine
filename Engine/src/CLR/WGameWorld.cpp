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
		GameWorld::Inst()->AddGameObject( entity->GetRef(), RString::ToString( name ));
		return entity;
	}

	WGameObject^ WGameWorld::RemoveGameObject( WGameObject^ entity )
	{
		GameWorld::Inst()->RemoveGameObject( entity->GetRef() );
		return entity;
	}

	WGameObject^ WGameWorld::GetGameObject( int index )
	{
		_ASSERT( index >= 0 );

		return gcnew WGameObject( GameWorld::Inst()->GetGameObject( index ));
	}

	int WGameWorld::NumGameObjects()
	{
		return (int)GameWorld::Inst()->NumGameObjects();
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
