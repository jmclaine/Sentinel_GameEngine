#include "WAssetManager.h"
#include "AssetManager.h"
#include "WString.h"

namespace Sentinel { namespace Utilities
{
	
#define DEFINE_ASSET_MANAGER( refClass )\
	W##refClass^ W##refClass##Manager::Add( System::String^ name, W##refClass^ data )\
	{ return gcnew W##refClass( refClass##Manager::Inst()->Add( WString::Cast( name ), data )); }\
	\
	void W##refClass##Manager::Remove( System::String^ name ) \
	{ refClass##Manager::Inst()->Remove( WString::Cast( name )); }\
	\
	W##refClass^ W##refClass##Manager::Get( System::String^ name )\
	{ return gcnew R##refClass( refClass##Manager::Inst()->Get( WString::Cast( name ))); }\
	\
	void W##refClass##Manager::GetAll( List< System::String^ >^% names, List< W##refClass^ >^% data )\
	{\
		std::vector< std::string > _names;\
		std::vector< std::shared_ptr< refClass >> _data;\
		refClass##Manager::Inst()->GetAll( _names, _data );\
		TRAVERSE_VECTOR( x, _names )\
			names->Add( WString::Cast( _names[ x ] ));\
		TRAVERSE_VECTOR( x, _data )\
			data->Add( gcnew R##refClass( _data[ x ] ));\
	}\
	void W##refClass##Manager::Create() { refClass##Manager::Inst( refClass##Manager::Create() ); }\
	bool W##refClass##Manager::Load( System::String^ filename ) { return refClass##Manager::Load( WString::Cast( filename ).c_str() ); }

	DEFINE_ASSET_MANAGER( Texture );
	DEFINE_ASSET_MANAGER( Shader );
}}
