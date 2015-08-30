#pragma once

#include "Sentinel.h"
#include "Component/Camera.h"
#include "Shape.h"

namespace Sentinel {
namespace Component
{
	class Transform;

	class SENTINEL_DLL Drawable : public GameComponent
	{
	protected:

		Transform* mTransform;

	public:

		BoundingBox mBounds;

		bool mIsDynamic;

	protected:

		Drawable();

	public:

		virtual void Startup();
		virtual void Update();
		virtual void Shutdown();

		/////////////////////////////

		virtual void Execute();

		void SetOwner(GameObject* owner);

		virtual void CalculateBounds() = 0;

		virtual bool CheckVisible(Camera* camera) = 0;

		virtual void Draw() = 0;

	protected:

		virtual DECLARE_SERIAL_SAVE();
		virtual DECLARE_SERIAL_LOAD();

		virtual void Copy(GameComponent* component);
	};
}}
