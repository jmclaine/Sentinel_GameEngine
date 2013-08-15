#pragma once

#include "m_shared_ptr.h"
#include "GameObject.h"
#include "WGameComponent.h"
#include "WString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Components
{
	public ref class WGameObject
	{
	private:

		GameObject*		mRef;

	public:

		WGameObject();
		WGameObject( GameObject* obj );
		WGameObject( WGameObject% obj );
		~WGameObject();
		!WGameObject();
		
		GameObject*		GetRef();

		WGameObject^	Parent();

		WString^		Name();

		//////////////////////////////

		WGameComponent^ AttachComponent( WGameComponent^ component, System::String^ name );

		void			DetachComponent( WGameComponent^ component );

		//////////////////////////////

		virtual void	Startup();

		virtual void	UpdateController();

		virtual void	UpdatePhysics();

		virtual void	UpdateDrawable();

		virtual void	UpdateComponents();

		virtual void	Shutdown();
	};
}}
