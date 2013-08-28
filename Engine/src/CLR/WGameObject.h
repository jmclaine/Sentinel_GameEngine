#pragma once
/*
Creates a new GameObject.
GameObject will not be freed unless it is attached to the GameWorld.
Delete() should be called manually for a controlled free memory.
*/
#include "m_shared_ptr.h"
#include "GameObject.h"
#include "WGameComponent.h"
#include "RString.h"

using namespace System;
using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	ref class RGameObject;

	public ref class WGameObject
	{
	protected:

		GameObject*		mRef;

	public:

		WGameObject();
		~WGameObject();

		void			Delete();
		
		GameObject*		GetRef();

		////////////////////////////////

		RGameObject^	Parent();

		RString^		Name();

		////////////////////////////////

		WGameComponent^ AttachComponent( WGameComponent^ component, String^ name );

		WGameComponent^	DetachComponent( WGameComponent^ component );

		////////////////////////////////

		virtual void	Startup();

		virtual void	UpdateController();

		virtual void	UpdatePhysics();

		virtual void	UpdateDrawable();

		virtual void	UpdateComponents();

		virtual void	Shutdown();
	};

	public ref class RGameObject sealed : public WGameObject
	{
	public:

		RGameObject( GameObject* obj );
		RGameObject( WGameObject^ obj );
	};
}}
