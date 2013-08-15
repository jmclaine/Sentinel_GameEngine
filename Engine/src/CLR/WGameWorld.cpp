#include "WGameWorld.h"
#include "WString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	void WGameWorld::Startup( System::String^ mapName )
	{
		GameWorld::Inst()->Startup( WString::ToString( mapName ));
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

	WGameObject^ WGameWorld::AddGameObject( WGameObject^ entity, System::String^ name )
	{
		GameWorld::Inst()->AddGameObject( entity->GetRef(), WString::ToString( name ));

		return entity;
	}

	void WGameWorld::RemoveGameObject( WGameObject^ entity )
	{
		GameWorld::Inst()->RemoveGameObject( entity->GetRef() );
	}

	//////////////////////////////

	void WGameWorld::SetCamera( UINT index )
	{
		GameWorld::Inst()->SetCamera( index );
	}
	/*
	WCameraComponent^ WGameWorld::GetCamera( int index = -1 )
	{
		return gcnew WCameraComponent( GameWorld::Inst()->GetCamera( index ));
	}

	WLightComponent^ WGameWorld::GetLight( UINT index )
	{
		return gcnew WLightComponent( GameWorld::Inst()->GetLight( index ));
	}
	*/
	WGameObject^ WGameWorld::GetGameObject( UINT index )
	{
		return gcnew WGameObject( GameWorld::Inst()->GetGameObject( index ));
	}
}}
