#pragma once

#include "Util.h"

// Declare variables for easy access as a property.
//
#define DECLARE_PROPERTY( varType, varName )\
	property varType varName\
	{\
		varType get();\
		void set( varType v );\
	}

#define DECLARE_PROPERTY_ARRAY( varType, varName, indexType )\
	property varType varName[ indexType ]\
	{\
		varType get( indexType i );\
		void set( indexType i, varType v );\
	}

// DEFINE PROPERTIES
// 
// Definitions:
//
// M = member (appends m to variable name)
// E = enum conversion
// B = base class exists (i.e., nested class)
// R = reference variables (i.e., converts to R class)
// S = static_cast required
// P = pointer based
// T = shared_ptr
//

// Native varTypes, e.g. int, float, double
//
#define DEFINE_PROPERTY( refClass, varType, varName )\
	void W##refClass::varName::set( varType v )\
	{\
		mRef->varName = v;\
	}\
	varType W##refClass::varName::get()\
	{\
		return mRef->varName;\
	}

// Member variables with native varTypes.
//
#define DEFINE_PROPERTY_M( refClass, varType, varName )\
	varType W##refClass::varName::get()\
	{\
		return mRef->m##varName;\
	}\
	void W##refClass::varName::set( varType v )\
	{\
		mRef->m##varName = v;\
	}

#define DEFINE_PROPERTY_MS( refClass, varType, varName )\
	varType W##refClass::varName::get()\
	{\
		return static_cast< refClass* >(mRef)->m##varName;\
	}\
	void W##refClass::varName::set( varType v )\
	{\
		static_cast< refClass* >(mRef)->m##varName = v;\
	}

#define DEFINE_PROPERTY_MB( baseClass, refClass, varType, varName )\
	varType W##baseClass::W##refClass::varName::get()\
	{\
		return mRef->m##varName;\
	}\
	void W##baseClass::W##refClass::varName::set( varType v )\
	{\
		mRef->m##varName = v;\
	}

// Member variables with enum varTypes.
//
#define DEFINE_PROPERTY_E( refClass, _namespace, varType, varName )\
	Sentinel::_namespace::varType W##refClass::varName::get()\
	{\
		return (Sentinel::_namespace::varType)mRef->m##varName;\
	}\
	void W##refClass::varName::set( Sentinel::_namespace::varType v )\
	{\
		mRef->m##varName = (Sentinel::varType)v;\
	}

#define DEFINE_PROPERTY_ES( refClass, _namespace, varType, varName )\
	Sentinel::_namespace::varType W##refClass::varName::get()\
	{\
		return (Sentinel::_namespace::varType)static_cast< refClass* >(mRef)->m##varName;\
	}\
	void W##refClass::varName::set( Sentinel::_namespace::varType v )\
	{\
		static_cast< refClass* >(mRef)->m##varName = (Sentinel::varType)v;\
	}

// Strings.
//
#define DEFINE_PROPERTY_STR( refClass, varName )\
	System::String^ W##refClass::varName::get()\
	{\
		return gcnew System::String( mRef->m##varName.c_str() );\
	}\
	void W##refClass::varName::set( System::String^ str )\
	{\
		mRef->m##varName = WString::Cast( str );\
	}

// Referenced variables.
//
#define DEFINE_PROPERTY_R( refClass, varType, varName )\
	W##varType^ W##refClass::varName::get()\
	{\
		return gcnew R##varType( &mRef->m##varName );\
	}\
	void W##refClass::varName::set( W##varType^ v )\
	{\
		mRef->m##varName = *v->GetRef();\
	}

#define DEFINE_PROPERTY_RS( refClass, varType, varName )\
	W##varType^ W##refClass::varName::get()\
	{\
		return gcnew R##varType( &static_cast< refClass* >(mRef)->m##varName );\
	}\
	void W##refClass::varName::set( W##varType^ v )\
	{\
		static_cast< refClass* >(mRef)->m##varName = *v->GetRef();\
	}

#define DEFINE_PROPERTY_RSB( baseClass, refClass, varType, varName )\
	W##varType^ W##baseClass::W##refClass::varName::get()\
	{\
		return gcnew R##varType( &static_cast< baseClass::refClass* >(mRef)->m##varName );\
	}\
	void W##baseClass::W##refClass::varName::set( W##varType^ v )\
	{\
		static_cast< baseClass::refClass* >(mRef)->m##varName = *v->GetRef();\
	}

// Pointer variables.
//
#define DEFINE_PROPERTY_RP( refClass, varType, varName )\
	W##varType^ W##refClass::varName::get()\
	{\
		return (mRef->m##varName) ? gcnew RP##varType( mRef->m##varName ) : nullptr;\
	}\
	void W##refClass::varName::set( W##varType^ v )\
	{\
		mRef->m##varName = v->GetRef();\
	}

#define DEFINE_PROPERTY_PS( refClass, varType, varName )\
	W##varType^ W##refClass::varName::get()\
	{\
		return (static_cast< refClass* >(mRef)->m##varName) ? gcnew W##varType( static_cast< refClass* >(mRef)->m##varName ) : nullptr;\
	}\
	void W##refClass::varName::set( W##varType^ v )\
	{\
		static_cast< refClass* >(mRef)->m##varName = v->GetRef();\
	}

#define DEFINE_PROPERTY_PRS( refClass, varType, varName )\
	W##varType^ W##refClass::varName::get()\
	{\
		return (static_cast< refClass* >(mRef)->m##varName) ? gcnew R##varType( static_cast< refClass* >(mRef)->m##varName ) : nullptr;\
	}\
	void W##refClass::varName::set( W##varType^ v )\
	{\
		static_cast< refClass* >(mRef)->m##varName = v->GetRef();\
	}

#define DEFINE_PROPERTY_RT( refClass, varType, varName )\
	W##varType^ W##refClass::varName::get()\
	{\
		return (mRef->m##varName) ? gcnew R##varType( mRef->m##varName ) : nullptr;\
	}\
	void W##refClass::varName::set( W##varType^ v )\
	{\
		mRef->m##varName = v->GetRef();\
	}

///////////////////////////////////////////////////

// Create standard wrapper reference for a class.
//
#define DECLARE_REF_PTR( refClass )\
	protected:\
		refClass* mRef;\
	public:\
		refClass* GetRef();

#define DEFINE_REF_PTR_EX( wrapClass, refClass )\
	refClass* wrapClass::GetRef() { return mRef; }

#define DEFINE_REF_PTR( refClass )\
	DEFINE_REF_PTR_EX( W##refClass, refClass );

#define DECLARE_REF_EX( wrapClass, refClass )\
	DECLARE_REF_PTR( refClass );\
	protected:\
		virtual void Release();\
	public:\
		~wrapClass();\
		!wrapClass();

#define DEFINE_REF_EX( wrapClass, refClass )\
	wrapClass::~wrapClass()			{ Release(); }\
	wrapClass::!wrapClass()			{ Release(); System::GC::SuppressFinalize( this ); }\
	void wrapClass::Release()		{ SAFE_DELETE( mRef ); }\
	DEFINE_REF_PTR_EX( wrapClass, refClass );

#define DEFINE_REF_EX_BASE( baseClass, wrapClass, refClass )\
	baseClass::wrapClass::~wrapClass()			{ Release(); }\
	baseClass::wrapClass::!wrapClass()			{ Release(); System::GC::SuppressFinalize( this ); }\
	void baseClass::wrapClass::Release()			{ SAFE_DELETE( mRef ); }\
	DEFINE_REF_PTR_EX( baseClass::wrapClass, refClass );

#define DECLARE_REF( refClass )\
	DECLARE_REF_EX( W##refClass, refClass );

#define DEFINE_REF( refClass )\
	DEFINE_REF_EX( W##refClass, refClass );

#define DECLARE_REF_SHARED( refClass )\
	protected:\
		m_shared_ptr< refClass >^ mRef;\
		virtual void Release();\
	public:\
		W##refClass( std::shared_ptr< refClass > obj );\
		~W##refClass();\
		!W##refClass();\
		std::shared_ptr< refClass > GetRef();

#define DEFINE_REF_SHARED( refClass )\
	void W##refClass::Release()									{ delete mRef; }\
	W##refClass::W##refClass( std::shared_ptr< refClass > obj ) { mRef = gcnew m_shared_ptr< refClass >( obj ); }\
	W##refClass::~W##refClass()									{ Release(); }\
	W##refClass::!W##refClass()									{ Release(); System::GC::SuppressFinalize( this ); }\
	std::shared_ptr< refClass > W##refClass::GetRef()			{ return mRef; }


// Create conversion operator for ease of use.
//
#define DECLARE_OP_DEREF( refClass )\
	operator const refClass& ();

#define DEFINE_OP_DEREF_EX( wrapClass, refClass )\
	wrapClass::operator const refClass& () { return *mRef; }

#define DEFINE_OP_DEREF( refClass )\
	DEFINE_OP_DEREF_EX( W##refClass, refClass );
	
#define DECLARE_OP_SHARED( refClass )\
	operator std::shared_ptr< refClass > ();

#define DEFINE_OP_SHARED_EX( wrapClass, refClass )\
	wrapClass::operator std::shared_ptr< refClass > () { return mRef; }

#define DEFINE_OP_SHARED( refClass )\
	DEFINE_OP_SHARED_EX( W##refClass, refClass );

#define DECLARE_OP_PTR( refClass )\
	operator refClass* ();

#define DEFINE_OP_PTR_EX( wrapClass, refClass )\
	wrapClass::operator refClass* () { return mRef; }

#define DEFINE_OP_PTR( refClass )\
	DEFINE_OP_PTR_EX( W##refClass, refClass );
	

// Create Reference class.
//
#define DECLARE_CLASS_REF( refClass )\
	public ref class R##refClass sealed : public W##refClass\
	{\
	public:\
		R##refClass( refClass* obj );\
		R##refClass( W##refClass^ obj );\
		void Set( const refClass& obj );\
		void Set( W##refClass^ obj );\
	protected:\
		virtual void Release() override;\
	};

#define DEFINE_CLASS_REF( refClass )\
	R##refClass::R##refClass( refClass* obj )		{ mRef = obj; }\
	R##refClass::R##refClass( W##refClass^ obj )	{ mRef = obj->GetRef(); }\
	void R##refClass::Set( const refClass& obj )	{ *mRef = obj; }\
	void R##refClass::Set( W##refClass^ obj )		{ *mRef = *obj->GetRef(); }\
	void R##refClass::Release() {}

#define DECLARE_CLASS_REF_SHARED( refClass )\
	public ref class R##refClass sealed : public W##refClass\
	{\
	private:\
		std::shared_ptr< refClass >& mRefPtr;\
	public:\
		R##refClass( std::shared_ptr< refClass >& obj );\
		void Set( W##refClass^ obj );\
	};

#define DEFINE_CLASS_REF_SHARED( refClass )\
	R##refClass::R##refClass( std::shared_ptr< refClass >& obj ) : mRefPtr( obj ), W##refClass( obj ) {}\
	void R##refClass::Set( W##refClass^ obj )\
	{ mRefPtr = obj->GetRef(); mRef = gcnew m_shared_ptr< refClass >(obj->GetRef()); }

#define DECLARE_CLASS_REF_PTR( refClass )\
	public ref class RP##refClass sealed : public W##refClass\
	{\
	private:\
		refClass*& mRefPtr;\
	public:\
		RP##refClass( refClass*& obj );\
		void Set( W##refClass^ obj );\
	protected:\
		virtual void Release() override;\
	};

#define DEFINE_CLASS_REF_PTR( refClass )\
	RP##refClass::RP##refClass( refClass*& obj ) : mRefPtr( obj ) { mRef = mRefPtr; }\
	void RP##refClass::Set( W##refClass^ obj )	{ mRefPtr = obj->GetRef(); mRef = mRefPtr; }\
	void RP##refClass::Release() {}

#define DECLARE_CLASS_REF_BASE( baseClass, refClass )\
	ref class R##refClass sealed : public W##refClass\
	{\
	public:\
		R##refClass( baseClass::refClass* obj );\
		R##refClass( W##refClass^ obj );\
		void Set( const baseClass::refClass& obj );\
		void Set( W##refClass^ obj );\
	protected:\
		virtual void Release() override;\
	};

#define DEFINE_CLASS_REF_BASE( baseClass, refClass )\
	W##baseClass::R##refClass::R##refClass( baseClass::refClass* obj )		{ mRef = obj; }\
	W##baseClass::R##refClass::R##refClass( W##refClass^ obj )				{ mRef = &*obj->GetRef(); }\
	void W##baseClass::R##refClass::Set( const baseClass::refClass& obj )	{ *mRef = obj; }\
	void W##baseClass::R##refClass::Set( W##baseClass::W##refClass^ obj )	{ *mRef = *obj->GetRef(); }\
	void W##baseClass::R##refClass::Release() {}
