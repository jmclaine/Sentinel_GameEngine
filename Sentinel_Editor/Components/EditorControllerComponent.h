#pragma once

#include "Component/Controller3D.h"
#include "Input.h"

namespace Sentinel
{
	namespace Component
	{
		class OrthographicCamera;
		class PerspectiveCamera;
	}

	class EditorControllerComponent : public Component::Controller3D
	{
	private:
		POINT mLastMousePos;

		float mLastWindowWidth;
		float mLastWindowHeight;

	public:
		float mForwardSpeed;
		float mStrafeSpeed;
		float mAngularSpeed;

		Component::OrthographicCamera* mEditorCamera;
		Component::PerspectiveCamera* mWorldCamera;

		//////////////////////////////

		EditorControllerComponent();

		void Startup();
		void Update();
		void Shutdown();
	};
}
