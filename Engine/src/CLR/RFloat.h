#pragma once
/*
A referenced float.
Used for public member variables of classes.
Allows setting as if variable were public.
*/
namespace Sentinel { namespace Utilities
{
	public ref class RFloat
	{
	private:

		float*	mRef;

	public:

		RFloat( float* value );
		RFloat( RFloat^ value );
		RFloat( RFloat% value );
		~RFloat();

		void Set( float value );
		void Set( RFloat^ value );

		static operator float( RFloat^ value );
	};
}}
