#pragma once

#include <vector>

template< class T, class U >
public ref class RStdVector
{
private:

	std::vector< T >&	mRef;

public:

	RStdVector( std::vector< T >& vec ) :
		mRef( vec ) 
	{}

	U^ Get( int loc )
	{
		return gcnew U( mRef[ loc ] );
	}

	void Add( U^ item )
	{
		mRef.push_back( item );
	}

	void Remove( int loc )
	{
		mRef.erase( mRef.begin() + loc );
	}

	void Insert( int loc, U^ item )
	{
		mRef.insert( mRef.begin() + loc, 1, item->GetRef() );
	}

	int Count()
	{
		return (int)mRef->size();
	}

	bool Empty()
	{
		return mRef->empty();
	}
};
