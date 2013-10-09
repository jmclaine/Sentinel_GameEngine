#include "Model.h"

namespace Sentinel
{
	Model::~Model()
	{}

	Model* Model::Load( const char* filename )
	{
		// Determine the extension of the object,
		// and load it according to its type.
		//
		int len = strlen( filename ) - 1;
		if( toupper(filename[ len - 2 ]) == 'O' && toupper(filename[ len - 1 ]) == 'B' && toupper(filename[ len ]) == 'J' )
		{
			return LoadModelOBJFromFile( filename );
		}
		else
		if( toupper(filename[ len - 2 ]) == 'M' && toupper(filename[ len - 1 ]) == '3' && toupper(filename[ len ]) == 'D' )
		{
			return LoadModelM3DFromFile( filename );
		}

		return NULL;
	}

	Model* Model::Load( Archive& archive )
	{
		BYTE format;
		archive.Read( &format );

		if( format == Model::OBJ )
			return LoadModelOBJFromArchive( archive );
		else
		if( format == Model::M3D )
			return LoadModelM3DFromArchive( archive );
		else
			return NULL;
	}
}