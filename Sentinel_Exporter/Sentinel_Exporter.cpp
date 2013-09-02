#include "stdafx.h"
#include "Sentinel_Exporter.h"

#define WRITE_ASCII		false
#define WRITE_TO_FILE( a, b ) {( WRITE_ASCII ) ? a : b;}

///////////////////////////////

MaxExporter::MaxExporter() :
	mGameScene( 0 ),
	mFile( 0 ),
	mIsWeighted( 0 )
{}

MaxExporter::~MaxExporter()
{
	deinit();
}

void MaxExporter::deinit()
{
	if ( mFile != 0 )
	{
		fclose( mFile );
		mFile = 0;
	}

	if ( mGameScene != 0 )
	{
		mGameScene->ReleaseIGame();
		mGameScene = 0;
	}
}

void MaxExporter::writeByte( BYTE& value )
{
	WRITE_TO_FILE( fprintf( mFile, "%d\n", value ),
				   fwrite( &value, sizeof(BYTE), 1, mFile ));
}

void MaxExporter::writeInt( int& num, bool is32bit )
{
	if( !is32bit )
	{
		WORD word = (unsigned short)num;
		WRITE_TO_FILE( fprintf( mFile, "%d\n", num ),
					   fwrite( &word, sizeof(WORD), 1, mFile ));
	}
	else
	{
		WRITE_TO_FILE( fprintf( mFile, "%d\n", num ),
					   fwrite( &num, sizeof(int), 1, mFile ));
	}
}

void MaxExporter::writeFloat( IGameProperty* prop )
{
	if( prop != NULL )
	{
		float value;
		prop->GetPropertyValue( value );
		short numf = FloatCompressor::compress( value );

		WRITE_TO_FILE( fprintf( mFile, "%f\n", value ), 
					   fwrite( &numf, sizeof(short), 1, mFile ));
	}
	else
	{
		float value = 0.0f;
		short numf = FloatCompressor::compress( value );

		WRITE_TO_FILE( fprintf( mFile, "%f\n", value ), 
					   fwrite( &numf, sizeof(short), 1, mFile ));
	}
}

void MaxExporter::writeFloat( float& num )
{
	short numf = FloatCompressor::compress( num );

	WRITE_TO_FILE( fprintf( mFile, "%f\n", num ), 
				   fwrite( &numf, sizeof(short), 1, mFile ));
}

void MaxExporter::writeFloat32( float& num )
{
	WRITE_TO_FILE( fprintf( mFile, "%f\n", num ), 
				   fwrite( &num, sizeof(float), 1, mFile ));
}

void MaxExporter::writeString( std::string& str )
{
	const char* data = str.c_str();
	WRITE_TO_FILE( fprintf( mFile, "%s\n", data ),
				   fprintf( mFile, "%s", data ));
}

void MaxExporter::writePoint2( Point2& point, bool is32bit  )
{
	if( !is32bit )
	{
		Point2S p;
		p.x = FloatCompressor::compress( point.x );
		p.y = FloatCompressor::compress( point.y );

		WRITE_TO_FILE( fprintf( mFile, "%f, %f\n", point.x, point.y ),
					   fwrite( &p, sizeof(p), 1, mFile ));
	}
	else
	{
		WRITE_TO_FILE( fprintf( mFile, "%f, %f\n", point.x, point.y ),
					   fwrite( &point, sizeof(point), 1, mFile ));
	}
}

void MaxExporter::writePoint3( IGameProperty* prop, bool is32bit  )
{
	Point3 point;

	if( prop != NULL )
	{
		prop->GetPropertyValue( point );
	}
	
	writePoint3( point, is32bit );
}

void MaxExporter::writePoint3( Point3& point, bool is32bit  )
{
	if( !is32bit )
	{
		Point3S p;
		p.x = FloatCompressor::compress( point.x );
		p.y = FloatCompressor::compress( point.y );
		p.z = FloatCompressor::compress( point.z );

		WRITE_TO_FILE( fprintf( mFile, "%f, %f, %f\n", point.x, point.y, point.z ), 
					   fwrite( &p, sizeof(p), 1, mFile ));
	}
	else
	{
		WRITE_TO_FILE( fprintf( mFile, "%f, %f, %f\n", point.x, point.y, point.z ), 
					   fwrite( &point, sizeof(point), 1, mFile ));
	}
}

void MaxExporter::writeFatIndex( FatIndex& index, bool is32bit )
{
	if( !is32bit )
	{
		FatIndexWORD indexW;
		indexW.vertex = (WORD)index.vertex;
		indexW.normal = (WORD)index.normal;
		indexW.texCoord = (WORD)index.texCoord;
	
		WRITE_TO_FILE( fprintf( mFile, "%d/%d/%d\n", indexW.vertex, indexW.normal, indexW.texCoord ),
					   fwrite( &indexW, sizeof(FatIndexWORD), 1, mFile ));
	}
	else
	{
		WRITE_TO_FILE( fprintf( mFile, "%d/%d/%d\n", index.vertex, index.normal, index.texCoord ),
					   fwrite( &index, sizeof(FatIndex), 1, mFile ));
	}
}

void MaxExporter::writeMatrix( GMatrix& mat )
{
	for( int z = 0; z < 4; ++z )
	{
		for( int w = 0; w < 4; ++w )
		{
			short numf = FloatCompressor::compress( mat[ z ][ w ] );

			WRITE_TO_FILE( fprintf( mFile, "%f ", mat[ z ][ w ] ), 
						   fwrite( &numf, sizeof(short), 1, mFile ));
		}

		if( WRITE_ASCII )
		{
			fprintf( mFile, "\n" );
		}
	}

	if( WRITE_ASCII )
	{
		fprintf( mFile, "\n" );
	}
}

void MaxExporter::writeMaterial( IGameMaterial* material )
{
	if( material->IsEntitySupported() )
	{
		IGameProperty* prop = NULL;

		// Ambient.
		//
		prop = material->GetAmbientData();
		if( prop != NULL )
		{
			writePoint3( prop );
		}
		else
		{
			writePoint3( Point3( 0.2f, 0.2f, 0.2f ));
		}

		// Diffuse.
		//
		prop = material->GetDiffuseData();
		if( prop != NULL )
		{
			writePoint3( prop );
		}
		else
		{
			writePoint3( Point3( 0.7f, 0.7f, 0.7f ));
		}
		
		// Specular.
		//
		prop = material->GetSpecularData();
		if( prop != NULL )
		{
			writePoint3( prop );
		}
		else
		{
			writePoint3( Point3( 0.1f, 0.1f, 0.1f ));
		}

		// Specular Level.
		//
		prop = material->GetSpecularLevelData();
		if( prop != NULL )
		{
			writeFloat( prop );
		}
		else
		{
			float spec_comp = 8.0f;
			writeFloat( spec_comp );
		}
	}
	else
	{
		// Ambient.
		//
		writePoint3( Point3( 0.2f, 0.2f, 0.2f ));
		
		// Diffuse.
		//
		writePoint3( Point3( 0.7f, 0.7f, 0.7f ));
		
		// Specular.
		//
		writePoint3( Point3( 0.1f, 0.1f, 0.1f ));
		
		// Specular Level.
		//
		float spec_comp = 8.0f;
		writeFloat( spec_comp );
	}
			
	// Textures.
	//
	int numTextures = material->GetNumberOfTextureMaps();
	std::vector< Texture > texNames;
	
	for( int x = 0; x < numTextures; ++x )
	{
		IGameTextureMap* texture = material->GetIGameTextureMap( x );
		if( texture != NULL )
		{
			// Ensure the texture is of a supported type.
			//
			int type = texture->GetStdMapSlot();
			if( type == -1 )
			{
				type = 0;	// assume diffuse
			}
			//if( type > 0 )//type == TEXTURE_DIFFUSE || type == TEXTURE_BUMP || type == TEXTURE_PARALLAX )
			//{
				// Store the texture filename only if it is a texture.
				//
				//char* className = texture->GetClassName();
				//if( strcmp( className, "Bitmap" ) == 0 )
				//{

			if( texture->IsEntitySupported() )
			{
				// Remove everything in the file name except for the actual image name with extension.
				//
				char* currName = texture->GetBitmapFileName();

				int len = (int)strlen( currName );
				int index = 0;

				for( int y = len-1; y > -1; --y )
				{
					if( currName[ y ] == '\\' )
					{
						index = y+1;
						break;
					}
				}

				std::string newName;
				for( int y = index; y < len; ++y )
				{
					newName.push_back( currName[ y ] );
				}
				texNames.push_back( Texture( newName, type ));
			}
			/*else
			{
				texNames.push_back( Texture( "", type ));
			}*/
		}
	}

	// Output filenames if they exist.
	//
	int count = (int)texNames.size();
	writeInt( count );
	
	for( int x = 0; x < count; ++x )
	{
		int len = (int)texNames[ x ].filename.size();

		writeInt( texNames[ x ].type );
		writeInt( len );
		writeString( texNames[ x ].filename );
	}
}

int MaxExporter::DoExport( const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressprompts, DWORD options )
{
	try
	{
		// Export only selected mObject.
		//
		const bool exportselected = 0 != (options & SCENE_EXPORT_SELECTED);

		// Open file for writing.
		//
		if( WRITE_ASCII )
		{
			fopen_s( &mFile, name, _T("w") );
		}
		else
		{
			fopen_s( &mFile, name, _T("wb") );
		}
		if ( !mFile )
		{
			throw std::exception( "Failed to open file for writing" );
		}
		
		// Initialize IGame.
		//
		mGameScene = GetIGameInterface();
		if ( !mGameScene )
		{
			throw std::exception( "Failed to initialize IGame interface" );
		}

		if( !mGameScene->InitialiseIGame( exportselected ))
		{
			throw std::exception( "Failed to initialize IGame" );
		}

		IGameConversionManager* cm = GetConversionManager();
		cm->SetCoordSystem( IGameConversionManager::IGAME_OGL );

		// Write version.
		//
		int version = (int)Version();
		writeInt( version );
		
		// Get the mMaterial.
		//
		int numMaterials = mGameScene->GetRootMaterialCount();
		for( int x = 0; x < numMaterials; ++x )
		{
			// Store the material data.
			//
			IGameMaterial* rootMaterial = mGameScene->GetRootMaterial( x );
			int numSubMaterials = rootMaterial->GetSubMaterialCount();
			
			// Store the root material only if no sub-mMaterial exist within it.
			//
			if( numSubMaterials == 0 )
			{
				mMaterial.push_back( rootMaterial );
			}
			// Store the sub-mMaterial.
			//
			else
			{
				IGameMaterial* subMaterial;
				for( int y = 0; y < numSubMaterials; ++y )
				{
					subMaterial = rootMaterial->GetSubMaterial( rootMaterial->GetMaterialID( y ));

					if( subMaterial != NULL )
					{
						mMaterial.push_back( subMaterial );
					}
				}
			}
		}

		// Write the mMaterial.
		//
		numMaterials = (int)mMaterial.size();
		
		writeInt( numMaterials );
		
		for( int x = 0; x < numMaterials; ++x )
		{
			writeMaterial( mMaterial[ x ] );
		}

		// Get the number of parent nodes.
		//
		int numNodes = mGameScene->GetTopLevelNodeCount();

		std::vector< IGameNode* > bones, dummy, mesh;

		for( int x = 0; x < numNodes; ++x )
		{
			IGameNode* node = mGameScene->GetTopLevelNode( x );
			IGameObject::ObjectTypes type = node->GetIGameObject()->GetIGameType();

			switch( type )
			{
				case IGameObject::IGAME_BONE:
					bones.push_back( node );
					break;

				case IGameObject::IGAME_MESH:
					dummy.push_back( node );
					break;

				case IGameObject::IGAME_UNKNOWN:
				case IGameObject::IGAME_HELPER:
				case IGameObject::IGAME_XREF:
					mesh.push_back( node );
					break;

				default:
					break;
			}
		}

		// Get BONES first!
		// Get data from each bone.
		//
		int count = (int)bones.size();
		for( int x = 0; x < count; ++x )
		{
			getNodeMesh( bones[ x ], 0, false );
		}

		// Get other dummies.
		//
		count = (int)dummy.size();
		for( int x = 0; x < count; ++x )
		{
			getNodeMesh( dummy[ x ] );
		}

		// Get data from each mesh.
		//
		count = (int)mesh.size();
		for( int x = 0; x < count; ++x )
		{
			getNodeMesh( mesh[ x ] );
		}

		// Find the smallest bounding sphere.
		//
		BoundingSphere mMinSphere = FindSmallestSphere( mVertex, (UINT)mVertex.size() );

		writePoint3( mMinSphere.center );
		writeFloat(  mMinSphere.radius );

		// Determine if these mVertex should include weights.
		//
		mIsWeighted = false;
		count = (int)mVertex.size();
		for( int x = 0; x < count; ++x )
		{
			if( mVertex[ x ].weights.size() > 0 )
			{
				mIsWeighted = true;
				break;
			}
		}
		
		// Write whether any data should be exported using 32-bit.
		//
		const BYTE WEIGHTED = 0x01;
		const BYTE VERTS_32 = 0x02;
		const BYTE NORMS_32 = 0x04;
		const BYTE TEXCS_32 = 0x08;
		const BYTE INDEX_32 = 0x10;

		bool bVerts32 = false;
		bool bNorms32 = false;
		bool bTexcs32 = false;
		bool bIndex32 = false;

		BYTE export32 = 0;
		if( mIsWeighted )
		{
			export32 |= WEIGHTED;
		}

		if( mVertex.size() > 65535 )
		{
			export32 |= VERTS_32;
			bVerts32 = true;
		}

		if( mNormal.size() > 65535 )
		{
			export32 |= NORMS_32;
			bNorms32 = true;
		}

		if( mTexCoord.size() > 65535 )
		{
			export32 |= TEXCS_32;
			bTexcs32 = true;
		}

		int numObjects = (int)mObject.size();
		for( int x = 0; x < numObjects; ++x )
		{
			int numFatIndices = (int)mObject[ x ].indices.size();
			for( int y = 0; y < numFatIndices; ++y )
			{
				if( (int)mObject[ x ].indices[ y ].size() > 65535 )
				{
					export32 |= INDEX_32;
					bIndex32 = true;

					y = numFatIndices;
					x = numObjects;
				}
			}
		}

		writeByte( export32 );

		// Write mVertex.
		//
		writeInt( count, bVerts32 );
		for( int x = 0; x < count; ++x )
		{
			writePoint3( mVertex[ x ].pos );

			if( mIsWeighted )
			{
				BYTE wCount = (BYTE)mVertex[ x ].weights.size();
				writeByte( wCount );
				for( int y = 0; y < (int)wCount; ++y )
				{
					writeInt( mVertex[ x ].weights[ y ].bone );		// possible crash on HUGE animated models
					writeFloat( mVertex[ x ].weights[ y ].weight );
				}
			}
		}
		
		// Write mNormal.
		//
		count = (int)mNormal.size();
		writeInt( count, bNorms32 );
		for( int x = 0; x < count; ++x )
		{
			writePoint3( mNormal[ x ] );
		}
		
		// Write texture coordinates.
		//
		count = (int)mTexCoord.size();
		writeInt( count, bTexcs32 );
		for( int x = 0; x < count; ++x )
		{
			writePoint2( mTexCoord[ x ] );
		}
		
		// Write fat indices, and keyframe animation.
		//
		numObjects = (int)mObject.size();
		writeInt( numObjects );
		for( int x = 0; x < numObjects; ++x )
		{
			// Hierarchy.
			//
			writeInt( mObject[ x ].hierarchy );

			// Skinned Object?
			//
			writeByte( mObject[ x ].isSkinned );

			// KeyFrames.
			//
			int numKeyFrames = (int)mObject[ x ].keyframe.size();
			writeInt( numKeyFrames );
			for( int y = 0; y < numKeyFrames; ++y )
			{
				// Export entire Matrix.
				//
				writeMatrix( mObject[ x ].keyframe[ y ].matWorld );

				// Export timestamp.
				//
				writeInt( mObject[ x ].keyframe[ y ].frame );
			}

			// Fat Indices.
			//
			int numFatIndices = (int)mObject[ x ].indices.size();
			writeInt( numFatIndices );
			for( int y = 0; y < numFatIndices; ++y )
			{
				int count = (int)mObject[ x ].indices[ y ].size();

				writeInt( mObject[ x ].matID[ y ] );

				writeInt( count, bIndex32 );
				for( int z = 0; z < count; ++z )
				{
					writeInt( mObject[ x ].indices[ y ][ z ].vertex,   bVerts32 );
					writeInt( mObject[ x ].indices[ y ][ z ].normal,   bNorms32 );
					writeInt( mObject[ x ].indices[ y ][ z ].texCoord, bTexcs32 );
				}
			}
		}

		// Release initialized stuff.
		//
		deinit();
	}
	catch ( std::exception& e )
	{
		TSTR tstr( e.what() );
		MessageBox( i->GetMAXHWnd(), tstr, _T( "Export Error" ), MB_OK|MB_ICONERROR );
		deinit();
	}

	return TRUE;
}

void MaxExporter::getNoMaterialMesh( IGameMesh* mesh, IGameSkin* skin, int currObjectIndex )
{
	UINT currFatIndex = (UINT)mObject[ currObjectIndex ].indices.size();

	mObject[ currObjectIndex ].matID.push_back( -1 );	// no material
	mObject[ currObjectIndex ].indices.push_back( std::vector< FatIndex >() );

	int numFaces = mesh->GetNumberOfFaces();
	for( int z = 0; z < numFaces; ++z )
	{
		FaceEx* face = mesh->GetFace( z );
		for( int w = 0; w < 3; ++w )
		{
			FatIndex index;
			Point3 point;
			Point2 texUV;
			int id;

			// Determine if this vertex already exists.
			//
			Vertex v;
			v.pos = mesh->GetVertex( face->vert[ w ], (skin) ? false : true );
			id = findVertex( v.pos );
			if( id == -1 )
			{
				id = (int)mVertex.size();

				// If this vertex has a skin, then get bones and weights.
				//
				if( skin != NULL )
				{
					DWORD index = face->vert[ w ];
					int numBones = skin->GetNumberOfBones( index );
					for( int x = 0; x < numBones && x < 4; ++x )
					{
						Weight weight;
						weight.weight = skin->GetWeight( index, x );

						IGameNode* bone = skin->GetIGameBone( index, x );
						weight.bone   = findBone( bone );

						int count = (int)v.weights.size();
						int j = 0;
						for(; j < count; ++j )
						{
							if( weight.weight > v.weights[ j ].weight )
							{
								break;
							}
						}
						v.weights.insert( v.weights.begin() + j, weight );
					}

					// Average the biggest four contributers.
					//
					float total = 0.0f;
					int count = (int)v.weights.size();
					for( int j = 0; j < count && j < 4; ++j )
					{
						total += v.weights[ j ].weight;
					}

					for( int j = 0; j < count && j < 4; ++j )
					{
						v.weights[ j ].weight /= total;
					}

					while( v.weights.size() > 4 )
					{
						v.weights.pop_back();
					}
				}
				mVertex.push_back( v );
			}
			index.vertex = id;
								
			// Determine if this normal already exists.
			//
			point = mesh->GetNormal( face->norm[ w ] );
			id = findObject( mNormal, point );
			if( id == -1 )
			{
				id = (int)mNormal.size();
				mNormal.push_back( point );
			}
			index.normal = id;
								
			// If this texture coordinate was found, write this ID instead of a new one.
			//
			texUV = mesh->GetTexVertex( face->texCoord[ w ] );
			id = findObject( mTexCoord, texUV );
			if( id == -1 )
			{
				id = (int)mTexCoord.size();
				mTexCoord.push_back( texUV );
			}
			index.texCoord = id;
								
			// Determine if this color already exists.
			//
			//point = mesh->GetColorVertex( faces[ z ]->color[ w ] );
			//id = -1;//findObject( mColor, point );
			//if( id == -1 )
			//{
			//	id = (int)mColor.size();
			//	mColor.push_back( point );
			//}
			//index.color = id;

			// Store the fat index.
			//
			mObject[ currObjectIndex ].indices[ currFatIndex ].push_back( index );
		}
	}
}

void MaxExporter::getNodeMesh( IGameNode* node, int hierarchy, bool doParentWT )
{
	IGameObject* obj = node->GetIGameObject();

	// Even if this object is not a mesh, it should export as a dummy.
	//
	int currObjectIndex = (int)mObject.size();
	mObject.push_back( Object() );

	mObject[ currObjectIndex ].node = node;

	// Store the hierarchy for parent referencing.
	//
	mObject[ currObjectIndex ].hierarchy = hierarchy;

	// Check if this object is a mesh.
	//
	if( obj->GetIGameType() == IGameObject::IGAME_MESH )
	{
		// Get Parent Mesh.
		//
		IGameMesh* mesh = (IGameMesh*)(obj);
		if( !mesh->InitializeData() )
		{
			throw std::exception( "Failed to initialize Parent Mesh" );
		}

		// Determine if this is a skinned mesh.
		//
		IGameSkin* skin = NULL;
		int numMods = mesh->GetNumModifiers();
		for( int x = 0; x < numMods; ++x )
		{
			if( mesh->GetIGameModifier( x )->IsSkin() )
			{
				if( mesh->IsObjectSkinned() )
				{
					skin = mesh->GetIGameSkin();
					mObject[ currObjectIndex ].isSkinned = 1;
					break;
				}
			}
		}

		// Get the active material IDs.
		//
		Tab< int > activeMatIDs = mesh->GetActiveMatIDs();

		// If no material, write the mesh as such.
		//
		if( activeMatIDs.Count() <= 0 )
		{
			getNoMaterialMesh( mesh, skin, currObjectIndex );
		}	
		// Store faces in the object as fat indices.
		//
		else
		for( int y = 0; y < activeMatIDs.Count(); ++y )
		{
			// Get the faces from this material ID.
			//
			Tab< FaceEx* > faces = mesh->GetFacesFromMatID( activeMatIDs[ y ] );
			IGameMaterial* mat = mesh->GetMaterialFromFace( faces[ 0 ] );
			
			int matID = findObject( mMaterial, mat );
					
			// If this material was found, store the faces.
			//
			if( matID > -1 )
			{
				// Prepare object to store the FatIndex.
				//
				UINT currFatIndex = (UINT)mObject[ currObjectIndex ].indices.size();

				mObject[ currObjectIndex ].matID.push_back( matID );
				mObject[ currObjectIndex ].indices.push_back( std::vector< FatIndex >() );

				// Store faces.
				//
				for( int z = 0; z < faces.Count(); ++z )
				{
					for( int w = 0; w < 3; ++w )
					{
						FatIndex index;
						Point3 point;
						Point2 texUV;
						int id;

						// Determine if this vertex already exists.
						//
						Vertex v;
						v.pos = mesh->GetVertex( faces[ z ]->vert[ w ], (skin) ? false : true );
						id = findVertex( v.pos );
						if( id == -1 )
						{
							id = (int)mVertex.size();

							// If this vertex has a skin, then get bones and weights.
							//
							if( skin != NULL )
							{
								DWORD index = faces[ z ]->vert[ w ];
								int numBones = skin->GetNumberOfBones( index );
								for( int x = 0; x < numBones && x < 4; ++x )
								{
									Weight weight;
									weight.weight = skin->GetWeight( index, x );

									IGameNode* bone = skin->GetIGameBone( index, x );
									weight.bone = findBone( bone );

									int count = (int)v.weights.size();
									int j = 0;
									for(; j < count; ++j )
									{
										if( weight.weight > v.weights[ j ].weight )
										{
											break;
										}
									}
									v.weights.insert( v.weights.begin() + j, weight );
								}

								// Average the biggest four contributers.
								//
								float total = 0.0f;
								int count = (int)v.weights.size();
								for( int j = 0; j < count && j < 4; ++j )
								{
									total += v.weights[ j ].weight;
								}

								for( int j = 0; j < count && j < 4; ++j )
								{
									v.weights[ j ].weight /= total;
								}

								while( v.weights.size() > 4 )
								{
									v.weights.pop_back();
								}
							}
							mVertex.push_back( v );
						}
						index.vertex = id;
								
						// Determine if this normal already exists.
						//
						point = mesh->GetNormal( faces[ z ]->norm[ w ] );
						id = findObject( mNormal, point );
						if( id == -1 )
						{
							id = (int)mNormal.size();
							mNormal.push_back( point );
						}
						index.normal = id;
								
						// If this texture coordinate was found, write this ID instead of a new one.
						//
						texUV = mesh->GetTexVertex( faces[ z ]->texCoord[ w ] );
						id = findObject( mTexCoord, texUV );
						if( id == -1 )
						{
							id = (int)mTexCoord.size();
							mTexCoord.push_back( texUV );
						}
						index.texCoord = id;
						
						// Determine if this color already exists.
						//
						//point = mesh->GetColorVertex( faces[ z ]->color[ w ] );
						//id = -1;//findObject( mColor, point );
						//if( id == -1 )
						//{
						//	id = (int)mColor.size();
						//	mColor.push_back( point );
						//}
						//index.color = id;

						// Store the fat index.
						//
						mObject[ currObjectIndex ].indices[ currFatIndex ].push_back( index );
					}
				}
			}
			else
			{
				getNoMaterialMesh( mesh, skin, currObjectIndex );
			}
		}
	}
	
	// Get the world transformation matrix for each keyframe.
	//
	TimeValue startTime = mGameScene->GetSceneStartTime();
	TimeValue endTime   = mGameScene->GetSceneEndTime();
	TimeValue dt        = 4800 / 30;

	GMatrix mat;
	int frame = 0;
	for( int t = startTime; t < endTime; t += dt )
	{
		if( hierarchy > 0 && doParentWT )
		{
			mat = node->GetWorldTM( t ) * node->GetNodeParent()->GetWorldTM( t ).Inverse();
		}
		else
		{
			mat = node->GetWorldTM( t );
		}

		// FIX ME!
		// This should export the frame number.
		//
		if( !mObject[ currObjectIndex ].keyframe.empty() )
		{
			if( mObject[ currObjectIndex ].keyframe.back().matWorld == mat )
			{}
			else
			{
				mObject[ currObjectIndex ].keyframe.push_back( KeyFrame( mat, frame ));
			}
		}
		else
		{
			mObject[ currObjectIndex ].keyframe.push_back( KeyFrame( mat, frame ));
		}
		++frame;
	}

	// Get the node mesh for each child.
	//
	int numChildren = node->GetChildCount();
	for( int y = 0; y < numChildren; ++y )
	{
		getNodeMesh( node->GetNodeChild( y ), hierarchy + 1, doParentWT );
	}
}

// Find the smallest bounding sphere.
// Based on code by:
// Nicolas Capens @ www.flipcode.com/archives/Smallest_Enclosing_Spheres.html
// Guest @ pastebin.com/04b1GBA2
//
BoundingSphere MaxExporter::FindSmallestSphere( std::vector< Vertex >& verts, UINT size )
{
	std::vector< Point3 > points;				// points of interest
	BoundingSphere sphere( verts[ 0 ].pos );	// final bounding sphere
	BoundingSphere newSphere[ 14 ];				// create possible spheres

	points.push_back( verts[ 0 ].pos );
	UINT currIndex = 0;
	while( currIndex < size )
	{
		Point3 pos = verts[ currIndex ].pos;
		bool isContained = false;

		for( UINT x = 0; x < points.size(); ++x )
		{
			if( points[ x ] == pos )
			{
				isContained = true;
			}
		}

		if( !isContained )
		{
			if( !CheckPointInSphere( verts[ currIndex ].pos, sphere ))
			{
				BoundingSphere mMinSphere;
				float minRadius = FLT_MAX;
				int index = -1;

				switch( points.size() )
				{
					case 1:
						mMinSphere = BoundingSphere( pos, points[ 0 ] );
						points.push_back( pos );
						break;

					case 2:
						{
							newSphere[ 0 ] = BoundingSphere( pos, points[ 0 ] );
							newSphere[ 1 ] = BoundingSphere( pos, points[ 1 ] );

							if( CheckPointInSphere( points[ 1 ], newSphere[ 0 ] ))
							{
								minRadius = newSphere[ 0 ].radius;
								index = 0;
							}
							else
							if( newSphere[ 1 ].radius < minRadius && 
								CheckPointInSphere( points[ 0 ], newSphere[ 1 ] ))
							{
								minRadius = newSphere[ 1 ].radius;
								index = 1;
							}

							if( index > -1 )
							{
								mMinSphere = newSphere[ index ];
								points[ 1 - index ] = pos;
							}
							else
							{
								mMinSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ] );
								points.push_back( pos );
							}
						}
						break;

					case 3:
						{
							for( UINT y = 0; y < (UINT)points.size(); ++y )
							{
								newSphere[ y ] = BoundingSphere( pos, points[ y ] );
							}
							newSphere[ 3 ] = BoundingSphere( pos, points[ 0 ], points[ 1 ] );
							newSphere[ 4 ] = BoundingSphere( pos, points[ 0 ], points[ 2 ] );
							newSphere[ 5 ] = BoundingSphere( pos, points[ 1 ], points[ 2 ] );

							if( CheckPointInSphere( points[ 1 ], newSphere[ 0 ] ) && 
								CheckPointInSphere( points[ 2 ], newSphere[ 0 ] ))
							{
								index = 0;
								minRadius = newSphere[ 0 ].radius;
							}

							if( newSphere[ 1 ].radius < minRadius && 
								CheckPointInSphere( points[ 0 ], newSphere[ 1 ] ) && 
								CheckPointInSphere( points[ 2 ], newSphere[ 1 ] ))
							{
								index = 1;
								minRadius = newSphere[ 1 ].radius;
							}

							if( newSphere[ 2 ].radius < minRadius && 
								CheckPointInSphere( points[ 0 ], newSphere[ 2 ] ) && 
								CheckPointInSphere( points[ 1 ], newSphere[ 2 ] ))
							{
								index = 2;
								minRadius = newSphere[ 2 ].radius;
							}

							if( newSphere[ 3 ].radius < minRadius && 
								CheckPointInSphere( points[ 2 ], newSphere[ 3 ] ))
							{
								index = 3;
								minRadius = newSphere[ 3 ].radius;
							}

							if( newSphere[ 4 ].radius < minRadius && 
								CheckPointInSphere( points[ 1 ], newSphere[ 4 ] ))
							{
								index = 4;
								minRadius = newSphere[ 4 ].radius;
							}

							if( newSphere[ 5 ].radius < minRadius && 
								CheckPointInSphere( points[ 0 ], newSphere[ 5 ] ))
							{
								index = 5;
								minRadius = newSphere[ 5 ].radius;
							}

							if( index > -1 )
							{
								mMinSphere = newSphere[ index ];

								switch( index )
								{
									case 0:
										points.resize( 2 );
										points[ 1 ] = pos;
										break;

									case 1:
										points.resize( 2 );
										points[ 0 ] = pos;
										break;

									case 2:
										points[ 0 ] = points[ 2 ];
										points.resize( 2 );
										points[ 1 ] = pos;
										break;

									case 3:
										points[ 2 ] = pos;
										break;

									case 4:
										points[ 1 ] = pos;
										break;

									case 5:
										points[ 0 ] = pos;
										break;
								}
							}
							else
							{
								mMinSphere = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 2 ] );
								points.push_back( pos );
							}
						}
						break;

					case 4:
						{
							for( UINT y = 0; y < 4; ++y )
							{
								newSphere[ y ] = BoundingSphere( pos, points[ y ] );
							}

							for( UINT y = 4; y < 7; ++y )
							{
								newSphere[ y ] = BoundingSphere( pos, points[ 0 ], points[ y - 3 ] );
							}

							newSphere[ 7 ]  = BoundingSphere( pos, points[ 1 ], points[ 2 ] );
							newSphere[ 8 ]  = BoundingSphere( pos, points[ 1 ], points[ 3 ] );
							newSphere[ 9 ]  = BoundingSphere( pos, points[ 2 ], points[ 3 ] );
							newSphere[ 10 ] = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 2 ] );
							newSphere[ 11 ] = BoundingSphere( pos, points[ 0 ], points[ 1 ], points[ 3 ] );
							newSphere[ 12 ] = BoundingSphere( pos, points[ 1 ], points[ 2 ], points[ 3 ] );
							newSphere[ 13 ] = BoundingSphere( pos, points[ 0 ], points[ 2 ], points[ 3 ] );

							// Check 2 point spheres.
							//
							if( CheckPointInSphere( points[ 1 ], newSphere[ 0 ] ) && 
								CheckPointInSphere( points[ 2 ], newSphere[ 0 ] ) && 
								CheckPointInSphere( points[ 3 ], newSphere[ 0 ] ))
							{
								index = 0;
								minRadius = newSphere[ 0 ].radius;
							}

							if( newSphere[ 1 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 1 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 1 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 1 ] ))
							{
								index = 1;
								minRadius = newSphere[ 1 ].radius;
							}

							if( newSphere[ 2 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 2 ] ) &&
								CheckPointInSphere( points[ 1 ], newSphere[ 2 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 2 ] ))
							{
								index = 2;
								minRadius = newSphere[ 2 ].radius;
							}

							if( newSphere[ 3 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 3 ] ) &&
								CheckPointInSphere( points[ 1 ], newSphere[ 3 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 3 ] ))
							{
								index = 3;
								minRadius = newSphere[ 3 ].radius;
							}

							// Check 3 point spheres.
							//
							if( newSphere[ 4 ].radius < minRadius &&
								CheckPointInSphere( points[ 2 ], newSphere[ 4 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 4 ] ))
							{
								index = 4;
								minRadius = newSphere[ 4 ].radius;
							}

							if( newSphere[ 5 ].radius < minRadius &&
								CheckPointInSphere( points[ 1 ], newSphere[ 5 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 5 ] ))
							{
								index = 5;
								minRadius = newSphere[ 5 ].radius;
							}

							if( newSphere[ 6 ].radius < minRadius &&
								CheckPointInSphere( points[ 1 ], newSphere[ 6 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 6 ] ))
							{
								index = 6;
								minRadius = newSphere[ 6 ].radius;
							}

							if( newSphere[ 7 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 7 ] ) &&
								CheckPointInSphere( points[ 3 ], newSphere[ 7 ] ))
							{
								index = 7;
								minRadius = newSphere[ 7 ].radius;
							}

							if( newSphere[ 8 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 8 ] ) &&
								CheckPointInSphere( points[ 2 ], newSphere[ 8 ] ))
							{
								index = 8;
								minRadius = newSphere[ 8 ].radius;
							}

							if( newSphere[ 9 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 9 ] ) &&
								CheckPointInSphere( points[ 1 ], newSphere[ 9 ] ))
							{
								index = 9;
								minRadius = newSphere[ 9 ].radius;
							}

							// Check 4 point spheres.
							//
							if( newSphere[ 10 ].radius < minRadius &&
								CheckPointInSphere( points[ 3 ], newSphere[ 10 ] ))
							{
								index = 10;
								minRadius = newSphere[ 10 ].radius;
							}

							if( newSphere[ 11 ].radius < minRadius &&
								CheckPointInSphere( points[ 2 ], newSphere[ 11 ] ))
							{
								index = 11;
								minRadius = newSphere[ 11 ].radius;
							}

							if( newSphere[ 12 ].radius < minRadius &&
								CheckPointInSphere( points[ 0 ], newSphere[ 12 ] ))
							{
								index = 12;
								minRadius = newSphere[ 12 ].radius;
							}

							if( newSphere[ 13 ].radius < minRadius &&
								CheckPointInSphere( points[ 1 ], newSphere[ 13 ] ))
							{
								index = 13;
								minRadius = newSphere[ 13 ].radius;
							}

							mMinSphere = newSphere[ index ];

							switch( index )
							{
								case 0:
									points.resize( 2 );
									points[ 1 ] = pos;
									break;

								case 1:
									points.resize( 2 );
									points[ 0 ] = pos;
									break;

								case 2:
									points[ 0 ] = points[ 2 ];
									points.resize( 2 );
									points[ 1 ] = pos;
									break;

								case 3:
									points[ 0 ] = points[ 3 ];
									points.resize( 2 );
									points[ 1 ] = pos;
									break;

								case 4:
									points.resize( 3 );
									points[ 2 ] = pos;
									break;

								case 5:
									points.resize( 3 );
									points[ 1 ] = pos;
									break;

								case 6:
									points[ 1 ] = points[ 3 ];
									points.resize( 3 );
									points[ 2 ] = pos;
									break;

								case 7:
									points[ 0 ] = points[ 2 ];
									points.resize( 3 );
									points[ 2 ] = pos;
									break;

								case 8:
									points[ 0 ] = points[ 3 ];
									points.resize( 3 );
									points[ 2 ] = pos;
									break;

								case 9:
									points[ 0 ] = points[ 3 ];
									points[ 1 ] = pos;
									points.resize( 3 );
									break;

								case 10:
									points[ 3 ] = pos;
									break;

								case 11:
									points[ 2 ] = pos;
									break;

								case 12:
									points[ 0 ] = pos;
									break;

								case 13:
									points[ 1 ] = pos;
									break;
							}
						}
						break;
				}

				if( mMinSphere.radius > sphere.radius )
				{
					sphere = mMinSphere;
					currIndex = 0;
					continue;
				}
			}
		}

		++currIndex;
	}

	return sphere;
}

// Bounding Sphere created by points.
// Based on code by:
// Nicolas Capens @ www.flipcode.com/archives/Smallest_Enclosing_Spheres.html
//
const float EPSILON	= 1e-4f;		// NOTE: To avoid numerical inaccuracies

BoundingSphere::BoundingSphere( const Point3& _center, float _radius ) :
	center( _center ),
	radius( _radius )
{}

BoundingSphere::BoundingSphere( const Point3& v0, const Point3& v1 )
{
	Point3 u1 = v1 - v0;
	Point3 u0 = u1 * 0.5f;

	radius = Length( u0 ) + EPSILON;
	center = v0 + u0;
}

BoundingSphere::BoundingSphere( const Point3& v0, const Point3& v1,  const Point3& v2 )
{
	Point3 u1 = v1 - v0;
	Point3 u2 = v2 - v0;
	
	float det = 2.0f * DotProd( CrossProd( u1, u2 ), CrossProd( u1, u2 ));

	// Guarantee a smallest sphere of two points.
	//
	if( det == 0 )
	{
		BoundingSphere sphere;
		float len01 = LengthSquared( u1 );
		float len02 = LengthSquared( u2 );
			
		if( len01 > len02 )
		{
			sphere = BoundingSphere( v0, v1 );
		}
		else
		{
			sphere = BoundingSphere( v0, v2 );
		}
			
		radius = sphere.radius;
		center = sphere.center;
		return;
	}

	Point3 u0 = ( CrossProd( CrossProd( u1, u2 ), u1 ) * LengthSquared( u2 ) +
				  CrossProd( u2, CrossProd( u1, u2 ))  * LengthSquared( u1 )) / det;

	radius = Length( u0 ) + EPSILON;
	center = v0 + u0;
}

BoundingSphere::BoundingSphere( const Point3& v0, const Point3& v1, const Point3& v2, const Point3& v3 )
{
	Point3 u1 = v1 - v0;
	Point3 u2 = v2 - v0;
	Point3 u3 = v3 - v0;

	float det = 2.0f * (u1[0]*(u2[1]*u3[2]-u2[2]*u3[1]) - u1[1]*(u2[0]*u3[2]-u2[2]*u3[0]) + u1[2]*(u2[0]*u3[1]-u2[1]*u3[0]));

	// Guarantee a smallest sphere of three points.
	//
	if( det == 0 )
	{
		BoundingSphere sphere;
		float tri012 = TriangleArea( v0, v1, v2 );
		float tri013 = TriangleArea( v0, v1, v3 );
		float tri023 = TriangleArea( v0, v2, v3 );
			
		if( tri012 > tri013 && tri012 > tri023 )
		{
			sphere = BoundingSphere( v0, v1, v2 );
		}
		else
		if( tri013 > tri012 && tri013 > tri023 )
		{
			sphere = BoundingSphere( v0, v1, v3 );
		}
		else
		{
			sphere = BoundingSphere( v0, v2, v3 );
		}

		radius = sphere.radius;
		center = sphere.center;
		return;
	}

	Point3 u0 = (CrossProd( u1, u2 ) * LengthSquared( u3 ) +
				 CrossProd( u3, u1 ) * LengthSquared( u2 ) +
				 CrossProd( u2, u3 ) * LengthSquared( u1 )) / det;

	radius = Length( u0 ) + EPSILON;
	center = v0 + u0;
}

// Find the area of a triangle.
// Based on Heron's Formula @ www.mathopenref.com/heronsformula.html
//
float TriangleArea( const Point3& pos0, const Point3& pos1, const Point3& pos2 )
{
	float a = Length( pos0 - pos1 );
	float b = Length( pos0 - pos2 );
	float c = Length( pos1 - pos2 );
	float p = (a + b + c) / 2;

	return sqrt( p * (p - a) * (p - b) * (p - c) );
}

// Check for a point in a sphere.
//
bool CheckPointInSphere( const Point3& v, const BoundingSphere& sphere )
{
	float d  = LengthSquared(v - sphere.center);
	float r2 = sphere.radius * sphere.radius;
		
	return ( d < r2 ) ? true : false;
}

/////////////////////////////////////////////////////////////////////////////////

void MaxExporter::ShowAbout( HWND hwnd )
{
}

int MaxExporter::ExtCount()
{
	return 1;
}

const TCHAR* MaxExporter::Ext( int /*n*/ )
{
	return _T( "m3d" );
}

const TCHAR* MaxExporter::LongDesc()
{
	return _T( "Sentinel 3D Model Max Exporter" );
}

const TCHAR* MaxExporter::ShortDesc()
{
	return _T( "Sentinel 3D Model" );
}

const TCHAR* MaxExporter::AuthorName()
{
	return _T( "" );
}

const TCHAR* MaxExporter::CopyrightMessage()
{
	return _T( "Copyright (C)" );
}

const TCHAR* MaxExporter::OtherMessage1()
{
	return _T( "" );
}

const TCHAR* MaxExporter::OtherMessage2()
{
	return _T( "" );
}

unsigned int MaxExporter::Version()
{
	return 102;
}

BOOL MaxExporter::SupportsOptions( int ext, DWORD options )
{
	return TRUE;
}
