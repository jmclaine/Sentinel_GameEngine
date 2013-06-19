#include <map>
#include <vector>
#include <fstream>
#include <sstream>

#include "Actor.h"
#include "Collision.h"
#include "MeshBuilder.h"
#include "Model.h"
#include "Util.h"
#include "Renderer.h"


namespace Engine
{
	class ModelOBJ : public Model
	{
	private:

		// Create materials to map to their string names.
		//
		typedef std::map< std::string, MeshBuilder* >  MeshBuilderMap;
		typedef std::pair< std::string, MeshBuilder* > MeshBuilderPair;

		Mesh** m_mesh;
		UINT   m_numMeshes;

	public:

		ModelOBJ( const char* filename )
		{
			load( filename );
		}

		//////////////////////////////////////////////////////////////////////////

		void setMaterial( const Material& material )
		{
			for( UINT x = 0; x < m_numMeshes; ++x )
			{
				m_mesh[ x ]->setMaterial( material );
			}
		}

		void setShader( Shader* shader )
		{
			for( UINT x = 0; x < m_numMeshes; ++x )
			{
				m_mesh[ x ]->setShader( shader );
			}
		}

		//////////////////////////////////////////////////////////////////////////

		// TODO:
		// Support animation for OBJ files.
		//
		// Set a keyframe, and append it if the index is -1.
		//
		void setKeyFrame( const KeyFrame& key, int keyIndex = -1, int objIndex = 0 )
		{}

		void setTime( float _time, UINT objIndex = 0 )
		{}

		float getTime( UINT objIndex = 0 )
		{
			return 0;
		}

		// Clear the geometry and materials.
		//
		void release()
		{
			for( UINT x = 0; x < m_numMeshes; ++x )
			{
				m_mesh[ x ]->release();
				delete m_mesh[ x ];
			}

			delete[] m_mesh;
		}

		// Load the model.
		//
		void load( const char* filename )
		{
			// Clear out any model that may have existed previously.
			//
			release();

			// Model data.
			// OBJ files start with the first index as 1,
			// so push a dummy into the vectors to align.
			//
			std::vector< vec3f > positions(1);
			std::vector< vec2f > texCoords(1);
			std::vector< vec3f > normals(1);

			// Initialize the minimum and maximum vertex positions for the bounding sphere.
			//
			vec3f minPosition( FLT_MAX, FLT_MAX, FLT_MAX ),
				  maxPosition( FLT_MIN, FLT_MIN, FLT_MIN );

			// Create a default material.
			//
			MeshBuilderMap builder;
			const std::string defaultMaterial = "~*Default*~";
			builder.insert( MeshBuilderPair( defaultMaterial, new MeshBuilder() ));

			MeshBuilder* mesh = builder.begin()->second;
			mesh->setShader( m_shaderColor );

			// Read the file.
			//
			std::string line;
			std::ifstream file( filename );

			if( file.is_open() )
			{
				while( file.good() )
				{
					getline( file, line );

					std::stringstream parsehelper( line );
					std::string token;

					parsehelper >> token;

					// Comment.
					//
					if( token == "#" )
					{
						continue;
					}
					// Vertex.
					//
					else
					if( token == "v" )
					{
						vec3f p;
						parsehelper >> p.x >> p.y >> p.z;
						positions.push_back( p );

						for(UINT x=0; x<3; ++x)
						{
							maxPosition[x] = MAX_VALUE( maxPosition[x], p[x] );
							minPosition[x] = MIN_VALUE( minPosition[x], p[x] );
						}
					}
					// Texture coordinates.
					//
					else
					if( token == "vt" )
					{
						vec2f p;
						parsehelper >> p.x >> p.y;
						p.y = 1 - p.y;				// Texture is reverse y-axis.
						texCoords.push_back( p );
					}
					// Vertex normals.
					//
					else
					if( token == "vn" )
					{
						vec3f p;
						parsehelper >> p.x >> p.y >> p.z;
						normals.push_back( p );
					}
					// Material.
					//
					else
					if( token == "mtllib" )
					{
						// The next token is the filename.
						//
						parsehelper >> token;

						// Begin parsing through this material file.
						//
						std::string mtlLine,
									mtlName;
						std::ifstream mtlFile( token.c_str() );

						if( mtlFile.is_open() )
						{
							while( mtlFile.good() )
							{
								getline( mtlFile, mtlLine );

								std::stringstream mtlParsehelper( mtlLine );
								std::string mtlToken;

								mtlParsehelper >> mtlToken;

								// Create a new material.
								//
								if( mtlToken == "newmtl" )
								{
									// Store the material name.
									//
									mtlParsehelper >> mtlName;
									builder.insert( MeshBuilderPair( mtlName, new MeshBuilder() ));

									MeshBuilder* mesh = builder.find( mtlName )->second;
									mesh->setShader( m_shaderColor );
								}
								// Load a texture.
								//
								else
								if( mtlToken == "map_Kd" )
								{
									const MeshBuilderMap::iterator& mtl_iter = builder.find( mtlName );

									if( mtl_iter != builder.end() )
									{
										mtlParsehelper >> mtlToken;
										MeshBuilder* mesh = mtl_iter->second;
										mesh->loadTexture( mtlToken.c_str() );
										mesh->setShader( m_shaderTexture );
									}
								}
							}
						}
					}
					// Use a loaded material.
					//
					else
					if( token == "usemtl" )
					{
						// Reference the new material.
						//
						std::string mtlName;
						parsehelper >> mtlName;

						const MeshBuilderMap::iterator& mtl_iter = builder.find( mtlName );

						// If the material was not found, reference the default material.
						//
						if( mtl_iter == builder.end() )
						{
							mesh = builder.find( defaultMaterial )->second;
						}
                        else
                        {
                            mesh = mtl_iter->second;
                        }
					}
					// Fat indices.
					//
					else
					if( token == "f" )
					{
						if( mesh == NULL )
						{
							REPORT_ERROR( "Invalid syntax for OBJ file.", "OBJ Load Error" );
							return;
						}

						UINT i = 0;
						for(;;)
						{
							// Convert this token into an index.
							//
							parsehelper >> token;

							// Check if the last token has already been read.
							//
							if( parsehelper.fail() )
							{
								break;
							}

							// Position.
							// Texture coord.
							// Normal.
							//
							vec3i vIndex;
							int currType = 0;
							std::string fatIndex;

							// Convert the fat index into the three parts.
							//
							for( UINT x=0; x < token.size(); ++x )
							{
								if( token[x] != '/')
								{
									fatIndex.push_back( token[x] );
								}
								else
								{
									vIndex[ currType ] = atoi( fatIndex.c_str() );
									fatIndex.clear();
									++currType;
								}
							}
							vIndex[ currType ] = atoi( fatIndex.c_str() );

							// Search for a duplicate vertex, and reference its index instead.
							//
							UINT currVertex = mesh->findVertex( positions[ vIndex[ 0 ]], texCoords[ vIndex[ 1 ]], normals[ vIndex[ 2 ]] );
							if( currVertex == UINT_MAX )
							{
								// Create this vertex.
								//
								currVertex = mesh->numVertices();

								mesh->addVertex( positions[ vIndex[ 0 ]] );
								mesh->setTexCoord0( texCoords[ vIndex[ 1 ]].x, texCoords[ vIndex[ 1 ]].y );
								mesh->setNormal( normals[ vIndex[ 2 ]] );
							}

							// Add the index.
							//
							mesh->addIndex( currVertex );
							++i;

							if( i > 3 )
							{
								// This is part of a triangle list, so add a couple more indices to complete the triangle.
								//
								UINT index = mesh->numIndices();
								mesh->addIndex( mesh->getIndex( index - 4 ));
								mesh->addIndex( mesh->getIndex( index - 2 ));
							}
						}
					}
				}

				file.close();

				// All the information for the model has been retrieved.
				// Now make the VBOs and IBOs for each material.
				//
				UINT i = 0;
				m_mesh = new Mesh*[ builder.size() ];

				TRAVERSE_LIST( it, builder )
				{
					if( it->second->numVertices() > 0 )
					{
						m_mesh[ i ] = it->second->buildMesh();
						++i;
					}

					delete it->second;
				}

				builder.clear();
			}
			else
			{
				REPORT_ERROR( "Failed to load " << filename, "OBJ Model Load Failure" );
			}
		}

		// Update the model by keyframe.
		//
		void update( float deltaTime )
		{}

		// Render the model.
		//
		void draw()
		{
			for( UINT x = 0; x < m_numMeshes; ++x )
			{
				m_mesh[ x ]->setWorldMatrix( m_matWorld );
				m_mesh[ x ]->draw();
			}
		}
	};

	// Create an OBJ Model Loader.
	//
	Model* LoadModelOBJ( const char* filename )
	{
		return new ModelOBJ( filename );
	}
}
