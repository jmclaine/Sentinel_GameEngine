#pragma once

#include "GUI/Widget.h"
#include "Vector3f.h"
#include "Quatf.h"
#include "Matrix4f.h"

namespace Sentinel { namespace GUI
{
	class SENTINEL_DLL ModelWidget : public Widget
	{
	public:

		virtual DECLARE_SERIAL_REGISTER_SAVE() = 0;

	protected:

		ModelWidget*		mParentModel;

		Matrix4f			mMatrixWorld;

	public:

		Vector3f			mPosition;
		Quatf				mOrientation;
		Vector3f			mScale;

	protected:

		ModelWidget();

	public:

		virtual ~ModelWidget();

		///////////////////////////////////

		void				Startup();

		void				Update();

		void				Shutdown();

		///////////////////////////////////

		const Matrix4f&		GetMatrixWorld();

		///////////////////////////////////

		void				Save( Archive& archive );
		void				Load( Archive& archive );
	};
}}
