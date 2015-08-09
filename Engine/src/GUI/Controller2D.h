#pragma once

#include "Component/Transform.h"
#include "Event.h"

namespace Sentinel
{
	class Archive;

namespace GUI
{
	class SENTINEL_DLL Controller2D : public GameComponent
	{
	protected:

		Component::Transform* mTransform;

	protected:

		bool			mIsOver;

	public:

		Event< void() > mActionEnter;
		Event< void() >	mActionOver;
		Event< void() > mActionExit;

	protected:

		Controller2D();

	public:

		virtual ~Controller2D();

		virtual void	Startup();

		virtual void	SerialSave( Archive& archive ) = 0;

		void			Save( Archive& archive );
		void			Load( Archive& archive );

		///////////////////////////////

		virtual void	Copy( GameComponent* component );
	};
}}
