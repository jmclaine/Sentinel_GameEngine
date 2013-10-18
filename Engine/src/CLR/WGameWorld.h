#pragma once

#include "Property.h"
#include "GameWorld.h"

namespace Sentinel { namespace Wrapped
{
	ref class WRenderer;
	ref class WPhysicsSystem;
	ref class WTiming;
	ref class WTextureManager;
	ref class WShaderManager;
	ref class WMeshManager;
	ref class WModelManager;
	ref class WAudioSourceManager;
	ref class WArchive;
	ref class WGameObject;
	ref class WCameraComponent;
	ref class WLightComponent;

	public ref class WGameWorld
	{
	protected:

		DECLARE_REF_PTR( GameWorld );

	public:

		WGameWorld();

		void				Release();

		DECLARE_OP_PTR( GameWorld );

		DECLARE_PROPERTY( WRenderer^,			XRenderer );
		DECLARE_PROPERTY( WPhysicsSystem^,		XPhysicsSystem );
		DECLARE_PROPERTY( WTiming^,				XTiming );
		DECLARE_PROPERTY( WTextureManager^,		XTextureManager );
		DECLARE_PROPERTY( WShaderManager^,		XShaderManager );
		DECLARE_PROPERTY( WMeshManager^,		XMeshManager );
		DECLARE_PROPERTY( WModelManager^,		XModelManager );
		DECLARE_PROPERTY( WAudioSourceManager^,	XAudioSourceManager );
		
		void				Save( WArchive^ archive );
		void				Load( WArchive^ archive );

		void				Startup();

		void				UpdateController();
		void				UpdatePhysics();
		void				UpdateTransform();
		void				UpdateComponents();
		void				UpdateDrawable();

		void				Shutdown();

		//////////////////////////////

		WGameObject^		AddGameObject( WGameObject^ entity );
		WGameObject^		AddGameObject( WGameObject^ entity, System::String^ name );

		WGameObject^		RemoveGameObject( WGameObject^ entity );

		WGameObject^		GetGameObject( UINT index );
		UINT				NumGameObjects();

		//////////////////////////////

		void				SetCamera( WCameraComponent^ camera );

		WCameraComponent^	GetCamera( int index );	// -1 = mCurrentCamera
		WLightComponent^	GetLight( UINT index );
	};
}}
