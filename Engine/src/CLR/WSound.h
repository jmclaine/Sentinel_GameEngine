#pragma once

#include "Property.h"
#include "m_shared_ptr.h"
#include "Sound.h"

namespace Sentinel { namespace Wrapped
{
	public ref class WSound
	{
		DECLARE_REF_SHARED( Sound );

	public:

		DECLARE_OP_SHARED( Sound );

		void	Play();

		void	Pause();

		void	Stop();
	};

	DECLARE_CLASS_REF_SHARED( Sound );
}}
