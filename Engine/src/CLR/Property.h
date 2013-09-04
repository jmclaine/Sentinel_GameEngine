#pragma once

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

// Native varTypes, e.g. int, float, double
//
#define DEFINE_PROPERTY( clazz, varType, varName )\
	void W##clazz::varName::set( varType v )\
	{\
		mRef->varName = v;\
	}\
	varType W##clazz::varName::get()\
	{\
		return mRef->varName;\
	}

// Member variables with native varTypes.
//
#define DEFINE_PROPERTY_M( clazz, varType, varName )\
	void W##clazz::varName::set( varType v )\
	{\
		mRef->m##varName = v;\
	}\
	varType W##clazz::varName::get()\
	{\
		return mRef->m##varName;\
	}

// Member variables with enum varTypes.
//
#define DEFINE_PROPERTY_E( clazz, _namespace, varType, varName )\
	void W##clazz::varName::set( Sentinel::_namespace::varType v )\
	{\
		mRef->m##varName = (Sentinel::varType)v;\
	}\
	Sentinel::_namespace::varType W##clazz::varName::get()\
	{\
		return (Sentinel::_namespace::varType)mRef->m##varName;\
	}

// Strings.
//
#define DEFINE_PROPERTY_S( clazz, varName )\
	String^ W##clazz::varName::get()\
	{\
		return gcnew String( mRef->m##varName.c_str() );\
	}\
	void W##clazz::varName::set( String^ str )\
	{\
		mRef->m##varName = RString::ToString( str );\
	}

// Referenced variables.
//
#define DEFINE_PROPERTY_R( clazz, varType, varName )\
	W##varType^ W##clazz::varName::get()\
	{\
		return gcnew R##varType( &static_cast< clazz* >(mRef)->m##varName );\
	}\
	void W##clazz::varName::set( W##varType^ v )\
	{\
		static_cast< clazz* >(mRef)->m##varName = *v->GetRef();\
	}

#define DEFINE_PROPERTY_RS( clazz, varType, varName )\
	W##varType^ W##clazz::varName::get()\
	{\
		return gcnew R##varType( &mRef->m##varName );\
	}\
	void W##clazz::varName::set( W##varType^ v )\
	{\
		mRef->m##varName = *v->GetRef();\
	}

// Pointer variables.
//
#define DEFINE_PROPERTY_P( clazz, varType, varName )\
	W##varType^ W##clazz::varName::get()\
	{\
		return (mRef->m##varName) ? gcnew R##varType( mRef->m##varName ) : nullptr;\
	}\
	void W##clazz::varName::set( W##varType^ v )\
	{\
		mRef->m##varName = v->GetRef();\
	}

///////////////////////////////////////////////////

// Create standard wrapper reference for a class.
//
#define DECLARE_REF( clazz )\
	protected:\
		clazz* mRef;\
		virtual void Delete();\
	public:\
		clazz* GetRef();\
		~W##clazz();\
		!W##clazz();

#define DEFINE_REF( clazz )\
	W##clazz::~W##clazz()		{ Delete(); }\
	W##clazz::!W##clazz()		{ Delete(); }\
	void W##clazz::Delete()		{ delete mRef; }\
	clazz* W##clazz::GetRef()	{ return mRef; }

#define DECLARE_REF_SHARED( clazz )\
	protected:\
		m_shared_ptr< clazz > mRef;\
	public:\
		W##clazz( clazz* obj );\
		W##clazz( std::shared_ptr< clazz > obj );\
		std::shared_ptr< clazz > GetRef();

#define DEFINE_REF_SHARED( clazz )\
	W##clazz::W##clazz( clazz* obj )					{ mRef = obj; }\
	W##clazz::W##clazz( std::shared_ptr< clazz > obj )	{ mRef = obj.get(); }\
	std::shared_ptr< clazz > W##clazz::GetRef()			{ return mRef.get(); }

// Create conversion operator for ease of use.
//
#define DECLARE_OP_DEREF( clazz )\
	operator const clazz& ();

#define DEFINE_OP_DEREF( clazz )\
	W##clazz::operator const clazz& () { return *mRef; }

#define DECLARE_OP_PTR( clazz )\
	operator clazz* ();

#define DEFINE_OP_PTR( clazz )\
	W##clazz::operator clazz* () { return mRef; }


// Create Reference class.
//
#define DECLARE_CLASS_REF( clazz )\
	public ref class R##clazz sealed : public W##clazz\
	{\
	public:\
		R##clazz( clazz* obj );\
		R##clazz( W##clazz^ obj );\
		void Set( const clazz& obj );\
		void Set( W##clazz^ obj );\
	protected:\
		virtual void Delete() override;\
	};

#define DEFINE_CLASS_REF( clazz )\
	R##clazz::R##clazz( clazz* obj )		{ mRef = obj; }\
	R##clazz::R##clazz( W##clazz^ obj )		{ mRef = obj->GetRef(); }\
	void R##clazz::Set( const clazz& obj )	{ *mRef = obj; }\
	void R##clazz::Set( W##clazz^ obj )		{ *mRef = *obj->GetRef(); }\
	void R##clazz::Delete() {}

#define DECLARE_CLASS_REF_PTR( clazz )\
	public ref class R##clazz sealed : public W##clazz\
	{\
	private:\
		clazz*&		mRefPtr;\
	public:\
		R##clazz( clazz*& obj );\
		void Set( clazz*& obj );\
		void Set( W##clazz^ obj );\
	protected:\
		virtual void Delete() override;\
	};

#define DEFINE_CLASS_REF_PTR( clazz )\
	R##clazz::R##clazz( clazz*& obj ) : mRefPtr( obj ) { Set( obj ); }\
	void R##clazz::Set( clazz*& obj )	{ mRefPtr = obj; mRef = mRefPtr; }\
	void R##clazz::Set( W##clazz^ obj )	{ mRefPtr = obj->GetRef(); mRef = mRefPtr; }\
	void R##clazz::Delete() {}
