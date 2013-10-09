#pragma once

#include "WArchive.h"
#include "WString.h"

using namespace Sentinel::Utilities;

#define DECLARE_ASSET_MANAGER( refClass )\
		static void Create();\
		static void Destroy();\
		static W##refClass^ Add( System::String^ name, W##refClass^ data );\
		static void Remove( System::String^ name );\
		static void RemoveAll();\
		static W##refClass^ Get( System::String^ name );\
		static void GetAll( System::Collections::Generic::List< System::String^ >^% names,\
							System::Collections::Generic::List< W##refClass^ >^% data );\
		static void Save( WArchive^ archive );\
		static void Load( WArchive^ archive );

#define DEFINE_ASSET_MANAGER( refClass )\
	void W##refClass##Manager::Create()\
	{ refClass##Manager::Inst( refClass##Manager::Create() ); }\
	\
	void W##refClass##Manager::Destroy()\
	{ refClass##Manager::Destroy(); }\
	\
	W##refClass^ W##refClass##Manager::Add( System::String^ name, W##refClass^ data )\
	{ return gcnew W##refClass( refClass##Manager::Inst()->Add( WString::Cast( name ), data )); }\
	\
	void W##refClass##Manager::Remove( System::String^ name )\
	{ refClass##Manager::Inst()->Remove( WString::Cast( name )); }\
	\
	void W##refClass##Manager::RemoveAll()\
	{ refClass##Manager::Inst()->RemoveAll(); }\
	\
	W##refClass^ W##refClass##Manager::Get( System::String^ name )\
	{ return gcnew W##refClass( refClass##Manager::Inst()->Get( WString::Cast( name ))); }\
	\
	void W##refClass##Manager::GetAll( System::Collections::Generic::List< System::String^ >^% names,\
									   System::Collections::Generic::List< W##refClass^ >^% data )\
	{\
		std::vector< std::string > _names;\
		std::vector< std::shared_ptr< refClass >> _data;\
		refClass##Manager::Inst()->GetAll( _names, _data );\
		TRAVERSE_VECTOR( x, _names )\
			names->Add( WString::Cast( _names[ x ] ));\
		TRAVERSE_VECTOR( x, _data )\
			data->Add( gcnew W##refClass( _data[ x ] ));\
	}\
	void W##refClass##Manager::Save( WArchive^ archive )\
	{ refClass##Manager::Inst()->Save( *archive->GetRef() ); }\
	void W##refClass##Manager::Load( WArchive^ archive )\
	{ refClass##Manager::Inst()->Load( *archive->GetRef() ); }
