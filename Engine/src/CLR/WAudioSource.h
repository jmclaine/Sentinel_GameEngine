#pragma once

#include "Property.h"
#include "m_shared_ptr.h"
#include "AudioSource.h"

namespace Sentinel { namespace Wrapped
{
	public ref class WAudioSource
	{
		DECLARE_REF_SHARED( AudioSource );

	public:

		DECLARE_OP_SHARED( AudioSource );

		void	Play();

		void	Pause();

		void	Stop();
	};

	DECLARE_CLASS_REF_SHARED( AudioSource );
}}
