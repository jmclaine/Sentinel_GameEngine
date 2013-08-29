#pragma once

#define DECLARE_PROPERTY( type, value )\
	property type value\
	{\
		type get();\
		void set( type v );\
	}

// Native types, e.g. int, float, double
//
#define DEFINE_PROPERTY( clazz, type, value )\
	void W##clazz::value::set( type v )\
	{\
		mRef->value = v;\
	}\
	type W##clazz::value::get()\
	{\
		return mRef->value;\
	}

// Member variables with native types.
//
#define DEFINE_PROPERTY_M( clazz, type, value )\
	void W##clazz::value::set( type v )\
	{\
		mRef->m##value = v;\
	}\
	type W##clazz::value::get()\
	{\
		return mRef->m##value;\
	}

// Member variables with enum types.
//
#define DEFINE_PROPERTY_E( clazz, _namespace, type, value )\
	void W##clazz::value::set( Sentinel::_namespace::type v )\
	{\
		mRef->m##value = (Sentinel::type)v;\
	}\
	Sentinel::_namespace::type W##clazz::value::get()\
	{\
		return (Sentinel::_namespace::type)mRef->m##value;\
	}

// Strings.
//
#define DEFINE_PROPERTY_S( clazz, value )\
	String^ W##clazz::value::get()\
	{\
		return gcnew String( mRef->m##value.c_str() );\
	}\
	void W##clazz::value::set( String^ str )\
	{\
		mRef->m##value = RString::ToString( str );\
	}

// Referenced variables.
//
#define DEFINE_PROPERTY_R( clazz, type, value )\
	W##type^ W##clazz::value::get()\
	{\
		return gcnew R##type( &static_cast< clazz* >(mRef)->m##value );\
	}\
	void W##clazz::value::set( W##type^ v )\
	{\
		static_cast< clazz* >(mRef)->m##value = *v->GetRef();\
	}

// Pointer variables.
//
#define DEFINE_PROPERTY_P( clazz, type, value )\
	W##type^ W##clazz::value::get()\
	{\
		return (mRef->m##value) ? gcnew R##type( mRef->m##value ) : nullptr;\
	}\
	void W##clazz::value::set( W##type^ v )\
	{\
		mRef->m##value = v->GetRef();\
	}
