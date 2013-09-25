#include "stdafx.h"
#include "Sentinel_Exporter.h"
#include "FileIO.h"

///////////////////////////////

MaxExporter::MaxExporter() :
	mGameScene( 0 ),
	mIsWeighted( 0 ),
	mFile( 0 )
{}

MaxExporter::~MaxExporter()
{
	deinit();
}

void MaxExporter::deinit()
{
	// 3ds Max does some sort of check on this
	// class before it allows it to be used.
	// If this deinit() function would cause
	// any sort of crash or exception, it will
	// cause the editor to crash or not display
	// the export file format.
	//
	if( mFile )
	{
		fclose( mFile );
		mFile = 0;
	}
	
	if( mGameScene != 0 )
	{
		mGameScene->ReleaseIGame();
		mGameScene = 0;
	}
}

void MaxExporter::WriteFloat( IGameProperty* prop )
{
	float value = 0.0f;

	if( prop != NULL )
		prop->GetPropertyValue( value );
	
	FileIO::Write( mFile, value );
}

void MaxExporter::WritePoint3( IGameProperty* prop, bool is32bit  )
{
	Point3 point;

	if( prop != NULL )
		prop->GetPropertyValue( point );
	
	FileIO::Write( mFile, (Vector3f)point, is32bit );
}

void MaxExporter::WriteFatIndex( FatIndex& index, bool is32bit )
{
	FileIO::Write( mFile, index.mVertex,   is32bit );
	FileIO::Write( mFile, index.mNormal,   is32bit );
	FileIO::Write( mFile, index.mTexCoord, is32bit );
}

void MaxExporter::WriteMatrix( GMatrix& mat )
{
	for( int z = 0; z < 4; ++z )
		for( int w = 0; w < 4; ++w )
			FileIO::Write( mFile, mat[ z ][ w ] );
}

void MaxExporter::WriteMaterial( IGameMaterial* material )
{
	if( material->IsEntitySupported() )
	{
		IGameProperty* prop = NULL;

		// Ambient.
		//
		prop = material->GetAmbientData();
		if( prop != NULL )
			WritePoint3( prop );
		else
			FileIO::Write( mFile, Vector3f( 0.2f, 0.2f, 0.2f ));
		
		// Diffuse.
		//
		prop = material->GetDiffuseData();
		if( prop != NULL )
			WritePoint3( prop );
		else
			FileIO::Write( mFile, Vector3f( 0.6f, 0.6f, 0.6f ));
		
		// Specular.
		//
		prop = material->GetSpecularData();
		if( prop != NULL )
			WritePoint3( prop );
		else
			FileIO::Write( mFile, Vector3f( 0.2f, 0.2f, 0.2f ));
		
		// Specular Component.
		//
		prop = material->GetSpecularLevelData();
		if( prop != NULL )
			WriteFloat( prop );
		else
			FileIO::Write( mFile, 8.0f );
	}
	else
	{
		// Ambient.
		//
		FileIO::Write( mFile, Vector3f( 0.2f, 0.2f, 0.2f ));
		
		// Diffuse.
		//
		FileIO::Write( mFile, Vector3f( 0.6f, 0.6f, 0.6f ));
		
		// Specular.
		//
		FileIO::Write( mFile, Vector3f( 0.2f, 0.2f, 0.2f ));
		
		// Specular Component.
		//
		FileIO::Write( mFile, 8.0f );
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
				type = 0;	// assume diffuse
			
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
					newName.push_back( currName[ y ] );

				texNames.push_back( Texture( newName, type ));
			}
		}
	}

	// Output filenames if they exist.
	//
	int count = (int)texNames.size();
	FileIO::Write( mFile, count );
	
	for( int x = 0; x < count; ++x )
	{
		int len = (int)texNames[ x ].mFilename.size();

		FileIO::Write( mFile, texNames[ x ].mType );
		FileIO::Write( mFile, len );
		FileIO::Write( mFile, texNames[ x ].mFilename.c_str() );
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
		mFile = fopen( name, "wb+" );
		
		if( !mFile )
			throw std::exception( "Failed to open file for writing" );
		
		// Initialize IGame.
		//
		mGameScene = GetIGameInterface();
		if ( !mGameScene )
			throw std::exception( "Failed to initialize IGame interface" );

		if( !mGameScene->InitialiseIGame( exportselected ))
			throw std::exception( "Failed to initialize IGame" );

		IGameConversionManager* cm = GetConversionManager();
		cm->SetCoordSystem( IGameConversionManager::IGAME_OGL );

		// Write version.
		//
		int version = (int)Version();
		FileIO::Write( mFile, version );
		
		// Get the materials.
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
			// Store the sub-material.
			//
			else
			{
				IGameMaterial* subMaterial;
				for( int y = 0; y < numSubMaterials; ++y )
				{
					subMaterial = rootMaterial->GetSubMaterial( rootMaterial->GetMaterialID( y ));

					if( subMaterial != NULL )
						mMaterial.push_back( subMaterial );
				}
			}
		}

		// Write the materials.
		//
		numMaterials = (int)mMaterial.size();
		
		FileIO::Write( mFile, numMaterials );
		
		for( int x = 0; x < numMaterials; ++x )
			WriteMaterial( mMaterial[ x ] );
		
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
			GetNodeMesh( bones[ x ], 0, false );
		
		// Get other dummies.
		//
		count = (int)dummy.size();
		for( int x = 0; x < count; ++x )
			GetNodeMesh( dummy[ x ] );
		
		// Get data from each mesh.
		//
		count = (int)mesh.size();
		for( int x = 0; x < count; ++x )
			GetNodeMesh( mesh[ x ] );

		// Find the smallest bounding sphere.
		//
		BoundingSphere mMinSphere( (UCHAR*)mVertex.data(), (UINT)mVertex.size(), sizeof( Vertex ));

		FileIO::Write( mFile, mMinSphere.mCenter );
		FileIO::Write( mFile, mMinSphere.mRadius );

		// Determine if these mVertex should include weights.
		//
		mIsWeighted = false;
		count = (int)mVertex.size();

		for( int x = 0; x < count; ++x )
			if( mVertex[ x ].mNumBones > 0 )
			{
				mIsWeighted = true;
				break;
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
			int numFatIndices = (int)mObject[ x ].mFatIndex.size();
			for( int y = 0; y < numFatIndices; ++y )
			{
				if( (int)mObject[ x ].mFatIndex[ y ].size() > 65535 )
				{
					export32 |= INDEX_32;
					bIndex32 = true;

					y = numFatIndices;
					x = numObjects;
				}
			}
		}

		FileIO::Write( mFile, export32 );

		// Write vertices.
		//
		FileIO::Write( mFile, count, bVerts32 );

		for( int x = 0; x < count; ++x )
		{
			FileIO::Write( mFile, (Vector3f)mVertex[ x ].mPosition );
			
			if( mIsWeighted )
			{
				BYTE wCount = (BYTE)mVertex[ x ].mNumBones;
				FileIO::Write( mFile, wCount );

				for( int y = 0; y < (int)wCount; ++y )
				{
					FileIO::Write( mFile, mVertex[ x ].mWeight[ y ].mBone );
					FileIO::Write( mFile, mVertex[ x ].mWeight[ y ].mAmount );
				}
			}
		}
		
		// Write normals.
		//
		count = (int)mNormal.size();
		FileIO::Write( mFile, count, bNorms32 );

		for( int x = 0; x < count; ++x )
			FileIO::Write( mFile, (Vector3f)mNormal[ x ] );

		// Write texture coordinates.
		//
		count = (int)mTexCoord.size();
		FileIO::Write( mFile, count, bTexcs32 );

		for( int x = 0; x < count; ++x )
			FileIO::Write( mFile, (Vector2f)mTexCoord[ x ] );
		
		// Write fat indices, and keyframe animation.
		//
		numObjects = (int)mObject.size();
		FileIO::Write( mFile, numObjects );

		for( int x = 0; x < numObjects; ++x )
		{
			// Hierarchy.
			//
			FileIO::Write( mFile, mObject[ x ].mHierarchy );

			// Skinned Object?
			//
			FileIO::Write( mFile, mObject[ x ].mIsSkinned );

			// KeyFrames.
			//
			int numKeyFrames = (int)mObject[ x ].mKeyFrame.size();
			FileIO::Write( mFile, numKeyFrames );

			for( int y = 0; y < numKeyFrames; ++y )
			{
				// Export matrix.
				//
				WriteMatrix( mObject[ x ].mKeyFrame[ y ].mMatrixWorld );

				// Export timestamp.
				//
				FileIO::Write( mFile, mObject[ x ].mKeyFrame[ y ].mFrame );
			}

			// Write fat indices.
			//
			int numFatIndices = (int)mObject[ x ].mFatIndex.size();
			FileIO::Write( mFile, numFatIndices );

			for( int y = 0; y < numFatIndices; ++y )
			{
				int count = (int)mObject[ x ].mFatIndex[ y ].size();

				FileIO::Write( mFile, mObject[ x ].mMaterialID[ y ] );

				FileIO::Write( mFile, count, bIndex32 );

				for( int z = 0; z < count; ++z )
				{
					FileIO::Write( mFile, mObject[ x ].mFatIndex[ y ][ z ].mVertex,   bVerts32 );
					FileIO::Write( mFile, mObject[ x ].mFatIndex[ y ][ z ].mNormal,   bNorms32 );
					FileIO::Write( mFile, mObject[ x ].mFatIndex[ y ][ z ].mTexCoord, bTexcs32 );
				}
			}
		}
	}
	catch( std::exception& e )
	{
		TSTR tstr( e.what() );
		MessageBox( i->GetMAXHWnd(), tstr, _T( "Export Error" ), MB_OK | MB_ICONERROR );
	}

	deinit();

	return TRUE;
}

void MaxExporter::CreateFatIndex( IGameMesh* mesh, FaceEx* face, IGameSkin* skin, std::vector< MaxExporter::FatIndex >& vFatIndex )
{
	for( int w = 0; w < 3; ++w )
	{
		FatIndex index;
		Point3   point;
		Point2   texUV;
		int		 id;

		// Determine if this vertex already exists.
		//
		Vertex vertex;
		vertex.mPosition = mesh->GetVertex( face->vert[ w ], (skin) ? false : true );
		id = FindVertex( vertex.mPosition );

		if( id == -1 )
		{
			id = (int)mVertex.size();

			// If this vertex has a skin, then get bones and weights.
			//
			if( skin != NULL )
			{
				DWORD index = face->vert[ w ];
				int numBones = skin->GetNumberOfBones( index );

				// Check each bone for the largest contributing weights
				// and allow no more than four.
				//
				for( int x = 0; x < numBones; ++x )
				{
					Weight weight;
					weight.mAmount = skin->GetWeight( index, x );

					IGameNode* bone = skin->GetIGameBone( index, x );
					weight.mBone = FindBone( bone );

					float largestWeight = 0;
					int   largestIndex = -1;
					for( int j = 3; j >= 0; --j )
					{
						if( weight.mAmount > vertex.mWeight[ j ].mAmount )
						{
							float diffWeight = weight.mAmount - vertex.mWeight[ j ].mAmount;
							if( diffWeight > largestWeight )
							{
								largestWeight = diffWeight;
								largestIndex  = j;
							}
						}
					}

					if( largestIndex > -1 )
						vertex.mWeight[ largestIndex ] = weight;
				}

				// Normalize the biggest four contributers.
				//
				float total = 0.0f;
					
				for( int j = 0; j < vertex.mNumBones; ++j )
					total += vertex.mWeight[ j ].mAmount;

				for( int j = 0; j < vertex.mNumBones; ++j )
					vertex.mWeight[ j ].mAmount /= total;
			}

			mVertex.push_back( vertex );
		}

		index.mVertex = id;
								
		// Determine if this normal already exists.
		//
		point = mesh->GetNormal( face->norm[ w ] );
		id = FindObject( mNormal, point );

		if( id == -1 )
		{
			id = (int)mNormal.size();
			mNormal.push_back( point );
		}

		index.mNormal = id;
								
		// If this texture coordinate was found, write this ID instead of a new one.
		//
		texUV = mesh->GetTexVertex( face->texCoord[ w ] );
		id = FindObject( mTexCoord, texUV );

		if( id == -1 )
		{
			id = (int)mTexCoord.size();
			mTexCoord.push_back( texUV );
		}

		index.mTexCoord = id;
								
		// Create FatIndex.
		//
		vFatIndex.push_back( index );
	}
}

void MaxExporter::GetNodeMesh( IGameNode* node, int hierarchy, bool doParentWT )
{
	IGameObject* obj = node->GetIGameObject();

	// Even if this object is not a mesh, it should export as a dummy.
	//
	int currObjectIndex = (int)mObject.size();
	mObject.push_back( Object() );

	mObject[ currObjectIndex ].mNode = node;

	// Store the hierarchy for parent referencing.
	//
	mObject[ currObjectIndex ].mHierarchy = hierarchy;

	// Check if this object is a mesh.
	//
	if( obj->GetIGameType() == IGameObject::IGAME_MESH )
	{
		// Get Parent Mesh.
		//
		IGameMesh* mesh = (IGameMesh*)(obj);

		if( !mesh->InitializeData() )
			throw std::exception( "Failed to initialize Parent Mesh" );

		// Determine if this is a skinned mesh.
		//
		IGameSkin* skin = NULL;
		int numMods = mesh->GetNumModifiers();

		for( int x = 0; x < numMods; ++x )
			if( mesh->GetIGameModifier( x )->IsSkin() )
				if( mesh->IsObjectSkinned() )
				{
					skin = mesh->GetIGameSkin();
					mObject[ currObjectIndex ].mIsSkinned = 1;
					break;
				}

		// Get the active material IDs.
		//
		Tab< int > activeMatIDs = mesh->GetActiveMatIDs();

		// If no material, write the mesh as such.
		//
		if( activeMatIDs.Count() <= 0 )
		{
			UINT currFatIndex = (UINT)mObject[ currObjectIndex ].mFatIndex.size();

			mObject[ currObjectIndex ].mMaterialID.push_back( -1 );
			mObject[ currObjectIndex ].mFatIndex.push_back( std::vector< FatIndex >() );

			int numFaces = mesh->GetNumberOfFaces();

			for( int z = 0; z < numFaces; ++z )
				CreateFatIndex( mesh,  mesh->GetFace( z ), skin, mObject[ currObjectIndex ].mFatIndex[ currFatIndex ] );
		}	
		// Store faces in the object as fat indices.
		//
		else
		for( int y = 0; y < activeMatIDs.Count(); ++y )
		{
			Tab< FaceEx* > faces = mesh->GetFacesFromMatID( activeMatIDs[ y ] );
			IGameMaterial* material = mesh->GetMaterialFromFace( faces[ 0 ] );
			
			int matID = FindObject( mMaterial, material );
					
			UINT currFatIndex = (UINT)mObject[ currObjectIndex ].mFatIndex.size();

			mObject[ currObjectIndex ].mMaterialID.push_back( matID );
			mObject[ currObjectIndex ].mFatIndex.push_back( std::vector< FatIndex >() );

			for( int z = 0; z < faces.Count(); ++z )
				CreateFatIndex( mesh, faces[ z ], skin, mObject[ currObjectIndex ].mFatIndex[ currFatIndex ] );
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
			mat = node->GetWorldTM( t ) * node->GetNodeParent()->GetWorldTM( t ).Inverse();
		else
			mat = node->GetWorldTM( t );

		if( !mObject[ currObjectIndex ].mKeyFrame.empty() )
		{
			if( !(mObject[ currObjectIndex ].mKeyFrame.back().mMatrixWorld == mat) )
				mObject[ currObjectIndex ].mKeyFrame.push_back( KeyFrame( mat, frame ));
		}
		else
		{
			mObject[ currObjectIndex ].mKeyFrame.push_back( KeyFrame( mat, frame ));
		}

		++frame;
	}

	// Get the node mesh for each child.
	//
	int numChildren = node->GetChildCount();

	for( int y = 0; y < numChildren; ++y )
		GetNodeMesh( node->GetNodeChild( y ), hierarchy + 1, doParentWT );
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
	return 103;
}

BOOL MaxExporter::SupportsOptions( int ext, DWORD options )
{
	return TRUE;
}
