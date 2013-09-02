#pragma once

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
