// Supports keyframes and skinning animations.
//
#include "stdafx.h"
#include "dll-examples.h"

#define WRITE_ASCII		false
#define WRITE_TO_FILE( a, b ) {( WRITE_ASCII ) ? a : b;}

MaxExporter::MaxExporter() :
	m_igame( 0 ),
	m_fh( 0 ),
	isWeighted( 0 )
{}

MaxExporter::~MaxExporter()
{
	deinit();
}

void MaxExporter::deinit()
{
	if ( m_fh != 0 )
	{
		fclose( m_fh );
		m_fh = 0;
	}

	if ( m_igame != 0 )
	{
		m_igame->ReleaseIGame();
		m_igame = 0;
	}
}

void MaxExporter::writeInt( int& num )
{
	WORD word = (unsigned short)num;
	WRITE_TO_FILE( fprintf( m_fh, "%d\n", num ),
				   fwrite( &word, sizeof(WORD), 1, m_fh ));
}

void MaxExporter::writeFloat( IGameProperty* prop )
{
	float value;
	prop->GetPropertyValue( value );
	short numf = FloatCompressor::compress( value );

	WRITE_TO_FILE( fprintf( m_fh, "%f\n", value ), 
				   fwrite( &numf, sizeof(short), 1, m_fh ));
}

void MaxExporter::writeFloat( float& num )
{
	short numf = FloatCompressor::compress( num );

	WRITE_TO_FILE( fprintf( m_fh, "%f\n", num ), 
				   fwrite( &numf, sizeof(short), 1, m_fh ));
}

void MaxExporter::writeFloat32( float& num )
{
	WRITE_TO_FILE( fprintf( m_fh, "%f\n", num ), 
				   fwrite( &num, sizeof(float), 1, m_fh ));
}

void MaxExporter::writeString( std::string& str )
{
	const char* data = str.c_str();
	WRITE_TO_FILE( fprintf( m_fh, "%s\n", data ),
				   fprintf( m_fh, "%s", data ));
}

void MaxExporter::writePoint2( Point2& point, bool is32bit  )
{
	if( !is32bit )
	{
		Point2S p;
		p.x = FloatCompressor::compress( point.x );
		p.y = FloatCompressor::compress( point.y );

		WRITE_TO_FILE( fprintf( m_fh, "%f, %f\n", point.x, point.y ),
					   fwrite( &p, sizeof(p), 1, m_fh ));
	}
	else
	{
		WRITE_TO_FILE( fprintf( m_fh, "%f, %f\n", point.x, point.y ),
					   fwrite( &point, sizeof(point), 1, m_fh ));
	}
}

void MaxExporter::writePoint3( IGameProperty* prop, bool is32bit  )
{
	Point3 point;
	prop->GetPropertyValue( point );
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

		WRITE_TO_FILE( fprintf( m_fh, "%f, %f, %f\n", point.x, point.y, point.z ), 
					   fwrite( &p, sizeof(p), 1, m_fh ));
	}
	else
	{
		WRITE_TO_FILE( fprintf( m_fh, "%f, %f, %f\n", point.x, point.y, point.z ), 
					   fwrite( &point, sizeof(point), 1, m_fh ));
	}
}

void MaxExporter::writeQuat( Quat& quat )
{
	float angle;
	Point3 axis;
	AngAxisFromQ( quat, &angle, axis);

	/*WRITE_TO_FILE( fprintf( m_fh, "%f, %f, %f, %f\n", axis.x, axis.y, axis.z, angle ), 
				   fwrite( &quat, sizeof(Quat), 1, m_fh ));*/

	writePoint3( axis );
	writeFloat( angle );
}

void MaxExporter::writeFatIndex( FatIndex& index )
{
	FatIndexWORD indexW;
	indexW.vert = (WORD)index.vertex;
	indexW.norm = (WORD)index.normal;
	indexW.texc = (WORD)index.texCoord;
	
	WRITE_TO_FILE( fprintf( m_fh, "%d/%d/%d\n", indexW.vert, indexW.norm, indexW.texc ),
				   fwrite( &indexW, sizeof(FatIndexWORD), 1, m_fh ));
}

void MaxExporter::writeMatrix( GMatrix& mat )
{
	for( int z = 0; z < 4; ++z )
	{
		for( int w = 0; w < 4; ++w )
		{
			short numf = FloatCompressor::compress( mat[ z ][ w ] );

			WRITE_TO_FILE( fprintf( m_fh, "%f ", mat[ z ][ w ] ), 
						   fwrite( &numf, sizeof(short), 1, m_fh ));
		}

		if( WRITE_ASCII )
		{
			fprintf( m_fh, "\n" );
		}
	}

	if( WRITE_ASCII )
	{
		fprintf( m_fh, "\n" );
	}
}

void MaxExporter::writeMaterial( IGameMaterial* material )
{
	// Ambient.
	//
	writePoint3( material->GetAmbientData() );

	// Diffuse.
	//
	writePoint3( material->GetDiffuseData() );
		
	// Specular.
	//
	writePoint3( material->GetSpecularData() );

	// Specular Level.
	//
	writeFloat( material->GetSpecularLevelData() );
			
	// Textures.
	//
	int numTextures = material->GetNumberOfTextureMaps();
	std::vector< Texture > texNames;
	
	for( int x = 0; x < numTextures; ++x )
	{
		IGameTextureMap* texture = material->GetIGameTextureMap( x );

		// Ensure the texture is of a supported type.
		//
		int type = texture->GetStdMapSlot();
		if( type == TEXTURE_DIFFUSE || type == TEXTURE_BUMP || type == TEXTURE_PARALLAX )
		{
			// Store the texture filename only if it is a texture.
			//
			char* className = texture->GetClassName();
			if( strcmp( className, "Bitmap" ) == 0 )
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
		// Export only selected objects.
		//
		const bool exportselected = 0 != (options & SCENE_EXPORT_SELECTED);

		// Open file for writing.
		//
		if( WRITE_ASCII )
		{
			fopen_s( &m_fh, name, _T("w") );
		}
		else
		{
			fopen_s( &m_fh, name, _T("wb") );
		}
		if ( !m_fh )
		{
			throw std::exception( "Failed to open file for writing" );
		}
		
		// Initialize IGame.
		//
		m_igame = GetIGameInterface();
		if ( !m_igame )
		{
			throw std::exception( "Failed to initialize IGame interface" );
		}

		if( !m_igame->InitialiseIGame( exportselected ))
		{
			throw std::exception( "Failed to initialize IGame" );
		}

		IGameConversionManager* cm = GetConversionManager();
		cm->SetCoordSystem( IGameConversionManager::IGAME_OGL );

		// Write version.
		//
		int version = (int)Version();
		writeInt( version );
		
		// Get the materials.
		//
		int numMaterials = m_igame->GetRootMaterialCount();

		for( int x = 0; x < numMaterials; ++x )
		{
			// Store the material data.
			//
			IGameMaterial* rootMaterial = m_igame->GetRootMaterial( x );
			int numSubMaterials = rootMaterial->GetSubMaterialCount();
			
			// Store the root material only if no sub-materials exist within it.
			//
			if( numSubMaterials == 0 )
			{
				materials.push_back( rootMaterial );
			}
			// Store the sub-materials.
			//
			else
			{
				IGameMaterial* subMaterial;
				for( int y = 0; y < numSubMaterials; ++y )
				{
					subMaterial = rootMaterial->GetSubMaterial( rootMaterial->GetMaterialID( y ));

					if( subMaterial != NULL )
					{
						materials.push_back( subMaterial );
					}
				}
			}
		}

		// Write the materials.
		//
		numMaterials = (int)materials.size();

		writeInt( numMaterials );
		
		for( int x = 0; x < numMaterials; ++x )
		{
			writeMaterial( materials[ x ] );
		}

		// Get the number of parent nodes.
		//
		int numNodes = m_igame->GetTopLevelNodeCount();

		std::vector< IGameNode* > bones, dummy, mesh;

		for( int x = 0; x < numNodes; ++x )
		{
			IGameNode* node = m_igame->GetTopLevelNode( x );
			IGameObject::ObjectTypes type = node->GetIGameObject()->GetIGameType();

			// Bones.
			//
			if( type == IGameObject::IGAME_BONE )
			{
				bones.push_back( node );
			}
			// Dummies.
			//
			else
			if( type != IGameObject::IGAME_MESH )
			{
				dummy.push_back( node );
			}
			// Meshes.
			//
			else
			{
				mesh.push_back( node );
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

		// Determine if these vertices should include weights.
		//
		count = (int)vertices.size();
		for( int x = 0; x < count; ++x )
		{
			if( vertices[ x ].weights.size() > 0 )
			{
				isWeighted = 1;
				break;
			}
		}
		
		writeInt( isWeighted );

		// Write vertices.
		//
		writeInt( count );
		for( int x = 0; x < count; ++x )
		{
			writePoint3( vertices[ x ].pos );

			if( isWeighted )
			{
				int wCount = (int)vertices[ x ].weights.size();
				writeInt( wCount );
				for( int y = 0; y < wCount; ++y )
				{
					writeInt( vertices[ x ].weights[ y ].bone );
					writeFloat( vertices[ x ].weights[ y ].weight );
				}
			}
		}
		
		// Write normals.
		//
		count = (int)normals.size();
		writeInt( count );
		for( int x = 0; x < count; ++x )
		{
			writePoint3( normals[ x ] );
		}
		
		// Write texture coordinates.
		//
		count = (int)texCoords.size();
		writeInt( count );
		for( int x = 0; x < count; ++x )
		{
			writePoint2( texCoords[ x ] );
		}
		
		// Write fat indices, and keyframe animation.
		//
		int numObjects = (int)objects.size();
		writeInt( numObjects );
		for( int x = 0; x < numObjects; ++x )
		{
			// Heirarchy.
			//
			writeInt( objects[ x ].heirarchy );

			// Skinned Object?
			//
			writeInt( objects[ x ].isSkinned );

			// KeyFrames.
			//
			int numKeyFrames = (int)objects[ x ].keyframe.size();
			writeInt( numKeyFrames );
			for( int y = 0; y < numKeyFrames; ++y )
			{
				/*
				writePoint3( objects[ x ].keyframe[ y ].m_position );
				writeQuat(   objects[ x ].keyframe[ y ].m_rotation );
				writePoint3( objects[ x ].keyframe[ y ].m_scale );
				*/

				// Export entire Matrix.
				//
				writeMatrix( objects[ x ].keyframe[ y ].m_world );

				// Export timestamp.
				//
				writeInt( objects[ x ].keyframe[ y ].m_frame );
			}

			// Fat Indices.
			//
			int numIndices = (int)objects[ x ].indices.size();
			writeInt( numIndices );
			for( int y = 0; y < numIndices; ++y )
			{
				int count = (int)objects[ x ].indices[ y ].size();

				writeInt( objects[ x ].matID[ y ] );
				writeInt( count );
				for( int z = 0; z < count; ++z )
				{
					writeFatIndex( objects[ x ].indices[ y ][ z ] );
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
	UINT currFatIndex = (UINT)objects[ currObjectIndex ].indices.size();

	objects[ currObjectIndex ].matID.push_back( -1 );	// no material
	objects[ currObjectIndex ].indices.push_back( std::vector< FatIndex >() );

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
				id = (int)vertices.size();

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
				vertices.push_back( v );
			}
			index.vertex = id;
								
			// Determine if this normal already exists.
			//
			point = mesh->GetNormal( face->norm[ w ] );
			id = findObject( normals, point );
			if( id == -1 )
			{
				id = (int)normals.size();
				normals.push_back( point );
			}
			index.normal = id;
								
			// If this texture coordinate was found, write this ID instead of a new one.
			//
			texUV = mesh->GetTexVertex( face->texCoord[ w ] );
			id = findObject( texCoords, texUV );
			if( id == -1 )
			{
				id = (int)texCoords.size();
				texCoords.push_back( texUV );
			}
			index.texCoord = id;
								
			//point = parentMesh->GetColorVertex( faces[ z ]->color[ w ] );

			// Store the fat index.
			//
			objects[ currObjectIndex ].indices[ currFatIndex ].push_back( index );
		}
	}
}

void MaxExporter::getNodeMesh( IGameNode* node, int heirarchy, bool doParentWT )
{
	IGameObject* obj = node->GetIGameObject();

	// Even if this object is not a mesh, it should export as a dummy.
	//
	int currObjectIndex = (int)objects.size();
	objects.push_back( Object() );

	objects[ currObjectIndex ].node = node;

	// Store the heirarchy for parent referencing.
	//
	objects[ currObjectIndex ].heirarchy = heirarchy;

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
					objects[ currObjectIndex ].isSkinned = 1;
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
			int matID = findObject( materials, mat );
					
			// If this material was found, store the faces.
			//
			if( matID > -1 )
			{
				// Prepare object to store the FatIndex.
				//
				UINT currFatIndex = (UINT)objects[ currObjectIndex ].indices.size();

				objects[ currObjectIndex ].matID.push_back( matID );
				objects[ currObjectIndex ].indices.push_back( std::vector< FatIndex >() );

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
							id = (int)vertices.size();

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
							vertices.push_back( v );
						}
						index.vertex = id;
								
						// Determine if this normal already exists.
						//
						point = mesh->GetNormal( faces[ z ]->norm[ w ] );
						id = findObject( normals, point );
						if( id == -1 )
						{
							id = (int)normals.size();
							normals.push_back( point );
						}
						index.normal = id;
								
						// If this texture coordinate was found, write this ID instead of a new one.
						//
						texUV = mesh->GetTexVertex( faces[ z ]->texCoord[ w ] );
						id = findObject( texCoords, texUV );
						if( id == -1 )
						{
							id = (int)texCoords.size();
							texCoords.push_back( texUV );
						}
						index.texCoord = id;
								
						//point = parentMesh->GetColorVertex( faces[ z ]->color[ w ] );

						// Store the fat index.
						//
						objects[ currObjectIndex ].indices[ currFatIndex ].push_back( index );
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
	TimeValue startTime = m_igame->GetSceneStartTime();
	TimeValue endTime   = m_igame->GetSceneEndTime();
	TimeValue dt        = 4800 / 30;

	GMatrix mat;
	int frame = 0;
	for( int t = startTime; t < endTime; t += dt )
	{
		if( heirarchy > 0 && doParentWT )
		{
			mat = node->GetWorldTM( t ) * node->GetNodeParent()->GetWorldTM( t ).Inverse();
		}
		else
		{
			mat = node->GetWorldTM( t );
		}

		/*
		KeyFrame keyframe;
		keyframe.m_position = mat.Translation();
		keyframe.m_rotation = mat.Rotation();
		keyframe.m_scale    = mat.Scaling();
		*/

		// FIX ME!
		// This should export the frame number.
		//
		if( !objects[ currObjectIndex ].keyframe.empty() )
		{
			if( objects[ currObjectIndex ].keyframe.back().m_world == mat )
			{}
			else
			{
				objects[ currObjectIndex ].keyframe.push_back( KeyFrame( mat, frame ));
			}
		}
		else
		{
			objects[ currObjectIndex ].keyframe.push_back( KeyFrame( mat, frame ));
		}
		++frame;
	}

	// Get the node mesh for each child.
	//
	int numChildren = node->GetChildCount();
	for( int y = 0; y < numChildren; ++y )
	{
		getNodeMesh( node->GetNodeChild( y ), heirarchy + 1, doParentWT );
	}
}

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
	return _T( "Max Exporter" );
}

const TCHAR* MaxExporter::ShortDesc()
{
	return _T( "McLaine Max Exporter" );
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