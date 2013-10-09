#pragma once

#include <memory>

// Based on http://stackoverflow.com/questions/6172361/c-cli-wrapping-a-function-that-returns-a-stdshared-ptr
// by Chillitom
//
template< class T >
public ref class m_shared_ptr sealed
{
private:

    std::shared_ptr< T >* mPtr;

	m_shared_ptr() :
		mPtr( new std::shared_ptr< T >() ) 
    {}

	void Release()
	{
		delete mPtr;
		mPtr = 0;
	}

public:

	m_shared_ptr( std::shared_ptr< T > t )
	{
        mPtr = new std::shared_ptr< T >(t);
    }

	m_shared_ptr( const m_shared_ptr< T >% t )
	{
        mPtr = new std::shared_ptr< T >(t);
    }

	~m_shared_ptr()
	{
        Release();
    }

    !m_shared_ptr()
	{
        Release();
		System::GC::SuppressFinalize( this );
    }

	std::shared_ptr< T >* GetPtr()
	{
		return mPtr;
	}

    operator std::shared_ptr< T >()
	{
        return *mPtr;
    }

	bool operator == ( m_shared_ptr< T >^ ptr )
	{
		return (*mPtr) == *(*ptr).mPtr;
	}

	m_shared_ptr< T >% operator = ( T* ptr )
	{
        mPtr = new std::shared_ptr< T >(ptr);
        return *this;
    }

	std::shared_ptr< T > operator->()
	{
		return *mPtr;
	}

	void reset()
	{
        mPtr->reset();
    }
};