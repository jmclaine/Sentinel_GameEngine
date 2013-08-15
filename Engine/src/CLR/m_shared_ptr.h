#pragma once

#include <memory>

template< class T >
public ref class m_shared_ptr sealed
{
private:

    std::shared_ptr< T >* mPtr;

public:

    m_shared_ptr() 
        : mPtr( new std::shared_ptr< T >() ) 
    {}

    m_shared_ptr( T* t )
	{
        mPtr = new std::shared_ptr< T >(t);
    }

    m_shared_ptr( std::shared_ptr< T > t )
	{
        mPtr = new std::shared_ptr< T >(t);
    }

    m_shared_ptr( const m_shared_ptr< T >% t )
	{
        mPtr = new std::shared_ptr< T >(*t.mPtr);
    }

	~m_shared_ptr()
	{
        delete mPtr;
    }

    !m_shared_ptr()
	{
        delete mPtr;
    }

    operator std::shared_ptr< T >()
	{
        return *mPtr;
    }

    m_shared_ptr< T >% operator = ( T* ptr )
	{
        mPtr = new std::shared_ptr< T >(ptr);
        return *this;
    }

	m_shared_ptr< T >% operator = ( m_shared_ptr< T > ptr )
	{
        mPtr = ptr.mPtr;
        return *this;
    }

	T* operator->()
	{
        return (*mPtr).get();
    }

	std::shared_ptr< T > get()
	{
		return (*mPtr);
	}

    void reset()
	{
        mPtr->reset();
    }
};