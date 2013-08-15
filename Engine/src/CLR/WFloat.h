#pragma once

// A float that references a value.
// Used for public member variables of classes.
// Allows getter to set as if variable were public.
//
namespace Sentinel { namespace Utilities
{
	public ref class WFloat
	{
	private:

		float*	mRef;

	public:

		WFloat( float* value );
		WFloat( WFloat^ value );
		WFloat( WFloat% value );
		~WFloat();

		void Set( WFloat^ value );
		void Set( float value );

		static operator float( WFloat^ value );
		static operator WFloat^( float value );
	};
}}
