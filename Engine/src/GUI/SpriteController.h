#pragma once

#include "GUI\Controller2D.h"
#include "Vector3.h"
#include "Point.h"

namespace Sentinel {
namespace GUI
{
	class SENTINEL_DLL SpriteController : public Controller2D
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

	protected:

		Vector3 mBasePosition;
		Vector3 mBaseScale;

	public:

		bool mPositionToWindowX;
		bool mPositionToWindowY;

		bool mScaleToWindowX;
		bool mScaleToWindowY;

		Quad mMargin;

	public:

		SpriteController();
		virtual ~SpriteController();

		///////////////////////////////////

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();

		///////////////////////////////////

		virtual GameComponent* Copy();

		virtual void Copy(GameComponent* component);
	};
}}
