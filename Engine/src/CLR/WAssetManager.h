#pragma once

#include "WArchive.h"
#include "WString.h"

#define DECLARE_ASSET_MANAGER( refClass )\
	private:\
		DECLARE_REF_PTR( refClass##Manager );\
	public:\
		W##refClass##Manager();\
		W##refClass##Manager( refClass##Manager* manager );\
		DECLARE_OP_PTR( refClass##Manager );\
		void Release();\
		W##refClass^ Add( System::String^ name, W##refClass^ data );\
		void Remove( System::String^ name );\
		void RemoveAll();\
		W##refClass^ Get( System::String^ name );\
		System::String^ Get( W##refClass^ data );\
		void GetAll( System::Collections::Generic::List< System::String^ >^% names,\
					 System::Collections::Generic::List< W##refClass^ >^% data );

#define DEFINE_ASSET_MANAGER( refClass )\
	DEFINE_REF_PTR( refClass##Manager );\
	\
	W##refClass##Manager::W##refClass##Manager()\
	{ mRef = new refClass##Manager(); }\
	\
	W##refClass##Manager::W##refClass##Manager( refClass##Manager* manager )\
	{ mRef = manager; }\
	\
	DEFINE_OP_PTR( refClass##Manager );\
	\
	void W##refClass##Manager::Release()\
	{ delete mRef; }\
	\
	W##refClass^ W##refClass##Manager::Add( System::String^ name, W##refClass^ data )\
	{ mRef->Add( WString::Cast( name ), data ); return data; }\
	\
	void W##refClass##Manager::Remove( System::String^ name )\
	{ mRef->Remove( WString::Cast( name )); }\
	\
	void W##refClass##Manager::RemoveAll()\
	{ mRef->RemoveAll(); }\
	\
	W##refClass^ W##refClass##Manager::Get( System::String^ name )\
	{ return gcnew W##refClass( mRef->Get( WString::Cast( name ))); }\
	\
	System::String^ W##refClass##Manager::Get( W##refClass^ data )\
	{ return WString::Cast( mRef->Get( data )); }\
	\
	void W##refClass##Manager::GetAll( System::Collections::Generic::List< System::String^ >^% names,\
									   System::Collections::Generic::List< W##refClass^ >^% data )\
	{\
		std::vector< std::string > _names;\
		std::vector< std::shared_ptr< refClass >> _data;\
		mRef->GetAll( _names, _data );\
		TRAVERSE_VECTOR( x, _names )\
			names->Add( WString::Cast( _names[ x ] ));\
		TRAVERSE_VECTOR( x, _data )\
			data->Add( gcnew W##refClass( _data[ x ] ));\
	}
