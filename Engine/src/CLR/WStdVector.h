#pragma once

#include <vector>

namespace Sentinel { namespace Wrapped
{
// Normally this class would be a template,
// but C# results in error CS1061.
// Macro fixes the issue.
//
// Creates variable as:
//
// WStdVector_extName
// RStdVector_extName
//
#define WStdVector_Class( extName, nativeClass, wrapClass, refClass )\
	public ref class WStdVector_##extName\
	{\
	protected:\
		std::vector< nativeClass >* mRef;\
		virtual void Release() { delete mRef; }\
	public:\
		WStdVector_##extName() { mRef = new std::vector< nativeClass >(); }\
		WStdVector_##extName( const std::vector< nativeClass >& obj )\
			{ mRef = new std::vector< nativeClass >( obj ); }\
		~WStdVector_##extName() { Release(); }\
		!WStdVector_##extName() { Release(); }\
		std::vector< nativeClass >* GetRef() { return mRef; }\
		operator const std::vector< nativeClass >& () { return *mRef; }\
		void		Set( int loc, wrapClass^ item ) { mRef->at( loc ) = item; }\
		refClass^	Get( int loc ) { return gcnew refClass( &mRef->at( loc ) ); }\
		void		Add( wrapClass^ item ) { mRef->push_back( item ); }\
		void		Remove( int loc ) { mRef->erase( mRef->begin() + loc ); }\
		void		Insert( int loc, wrapClass^ item ) { mRef->insert( mRef->begin() + loc, 1, item ); }\
		void		Clear() { mRef->clear(); }\
		int			Count() { return (int)mRef->size(); }\
		bool		Empty() { return mRef->empty(); }\
	};

#define RStdVector_Class( extName, nativeClass, wrapClass, refClass )\
	public ref class RStdVector_##extName sealed : public WStdVector_##extName\
	{\
	protected:\
		virtual void Release() override {}\
	public:\
		RStdVector_##extName( std::vector< nativeClass >* vec ) { Set( vec ); }\
		void		Set( std::vector< nativeClass >* vec ) { mRef = vec; }\
	};

/////////////////////////////////////////////

#define WStdVector_Native( extName, nativeVar, refVar )\
	public ref class WStdVector_##extName\
	{\
	protected:\
		std::vector< nativeVar >* mRef;\
		virtual void Release() { delete mRef; }\
	public:\
		WStdVector_##extName() { mRef = new std::vector< nativeVar >(); }\
		WStdVector_##extName( const std::vector< nativeVar >& obj )\
			{ mRef = new std::vector< nativeVar >( obj ); }\
		~WStdVector_##extName() { Release(); }\
		!WStdVector_##extName() { Release(); }\
		std::vector< nativeVar >* GetRef() { return mRef; }\
		operator const std::vector< nativeVar >& () { return *mRef; }\
		void		Set( int loc, refVar item ) { mRef->at( loc ) = item; }\
		refVar		Get( int loc ) { return (refVar)mRef->at( loc ); }\
		void		Add( refVar item ) { mRef->push_back( (nativeVar)item ); }\
		void		Remove( int loc ) { mRef->erase( mRef->begin() + loc ); }\
		void		Insert( int loc, refVar item ) { mRef->insert( mRef->begin() + loc, 1, (nativeVar)item ); }\
		void		Clear() { mRef->clear(); }\
		int			Count() { return (int)mRef->size(); }\
		bool		Empty() { return mRef->empty(); }\
	};

#define RStdVector_Native( extName, nativeVar, refVar )\
	public ref class RStdVector_##extName sealed : public WStdVector_##extName\
	{\
	protected:\
		virtual void Release() override {}\
	public:\
		RStdVector_##extName( std::vector< nativeVar >* vec ) { Set( vec ); }\
		void		Set( std::vector< nativeVar >* vec ) { mRef = vec; }\
	};

	/*
	// Causes error CS1061.
	// 
	template< class T, class U >
	public ref class RStdVector sealed
	{
	private:

		std::vector< T >*	mRef;

	public:

		RStdVector( std::vector< T >* vec )
		{
			Set( vec );
		}

		void Set( std::vector< T >* vec )
		{
			mRef = vec;
		}

		U^ Get( int loc )
		{
			return gcnew U( &mRef->at( loc ) );
		}

		void Add( U^ item )
		{
			mRef->push_back( item );
		}

		void Remove( int loc )
		{
			mRef->erase( mRef->begin() + loc );
		}

		void Insert( int loc, U^ item )
		{
			mRef->insert( mRef->begin() + loc, 1, item );
		}

		void Clear()
		{
			mRef->clear();
		}

		int Count()
		{
			return (int)mRef->size();
		}

		bool Empty()
		{
			return mRef->empty();
		}
	};*/
}}
