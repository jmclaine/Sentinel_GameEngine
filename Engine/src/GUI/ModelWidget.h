#pragma once

#include "GUI/Widget.h"
#include "Vector3f.h"
#include "Quatf.h"
#include "Matrix4f.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL ModelWidget : public Widget
	{
		DECLARE_SERIAL();

	public:

		DECLARE_SERIAL_REGISTER_SAVE();

	protected:

		ModelWidget*		mParentModel;

		Matrix4f			mMatrixWorld;

	public:

		Vector3f			mPosition;
		Quatf				mOrientation;
		Vector3f			mScale;

		///////////////////////////////////

		ModelWidget();
		virtual ~ModelWidget();

		///////////////////////////////////

		void				Startup();

		void				Update();

		void				Shutdown();

		///////////////////////////////////

		const Matrix4f&		GetMatrixWorld();
	};
}}
