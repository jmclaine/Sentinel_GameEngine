// Reads a script from a file, and performs functions.
//
#include <string>
#include <list>
#include <iostream>
#include <windows.h>

#include "Util.h"
#include "Script.h"

namespace Sentinel
{
	#define REPORT_PARSER_ERROR( linenumber, title, msg, ... )\
		didError = true;\
		char buf[ 256 ];\
		sprintf_s( buf, msg, __VA_ARGS__ );\
		REPORT_SCRIPT_ERROR( linenumber, title, buf );

	ScriptParser::ScriptParser()
	{
		Startup();
	}

	ScriptParser::ScriptParser( const char* filename )
	{
		Startup();
		Load( filename );
	}

	void ScriptParser::Startup()
	{
		currVars = NULL;
		didError = false;
	}

	int ScriptParser::Load( const char* filename )
	{
		try
		{
			if( lex.Load( filename ) == 0 )
			{
				lex.Shutdown();
				throw( ScriptException( 0, "LOAD_FAILURE", "Failed to load script." ));
			}

			lex.Next();
			while( lex.token != TOKEN_EOF )
			{
				ScriptNode* node = ParseStatement();
				
				if( node != NULL )
				{
					nodes.push_back( node );
				}
				else
				{
					lex.Next();
				}

				if( didError )
				{
					throw( ScriptException( lex.linenumber, "PARSER_ERROR", "Failed to parse script." ));
				}

				currVars = NULL;
			}
		}
		catch( ScriptException& e )
		{
			Shutdown();
			OutputDebugStringA( e.what() );
			return 0;
		}

		lex.Shutdown();
		currVars = NULL;

		return 1;
	}

	void ScriptParser::Shutdown()
	{
		lex.Shutdown();
		for( int x = 0; x < (int)nodes.size(); ++x )
		{
			nodes[ x ]->Shutdown();
			delete nodes[ x ];
		}
		nodes.clear();
	}

	void ScriptParser::Output( std::fstream& file )
	{
		for( int x = 0; x < (int)nodes.size(); ++x )
		{
			nodes[ x ]->Output( file );
		}
	}

	void ScriptParser::Expect( int token )
	{
		if( lex.token != token )
		{
			REPORT_PARSER_ERROR( lex.linenumber, "EXPECTED", TokenToString( token ));
		}
		lex.Next();
	}

	ScriptNode* ScriptParser::ParseStatement()
	{
		ScriptNode* tempNode = NULL;
		ScriptNode* currNode = NULL;
		int token;

		switch( lex.token )
		{
			case TOKEN_IF:
				tempNode = new ScriptNode( TOKEN_IF );

				lex.Next();
				Expect( TOKEN_LPAR );
				tempNode->sibling = ParsePlusExp();
				Expect( TOKEN_RPAR );

				tempNode->child.child = new ScriptNode[ 2 ];	// statement/else

				SetNode( tempNode->child.child[ 0 ], ParseStatement() );
				if( lex.token == TOKEN_ELSE )
				{
					lex.Next();
					SetNode( tempNode->child.child[ 1 ], ParseStatement() );
				}
				
				return tempNode;

			case TOKEN_WHILE:
				tempNode = new ScriptNode( TOKEN_WHILE );

				lex.Next();
				Expect( TOKEN_LPAR );
				tempNode->sibling = ParsePlusExp();
				Expect( TOKEN_RPAR );
				tempNode->child = SetData( ParseStatement() );

				return tempNode;

			case TOKEN_FOR:
				tempNode = new ScriptNode( TOKEN_FOR );

				lex.Next();
				Expect( TOKEN_LPAR );

				tempNode->sibling = new ScriptNode[ 3 ];

				SetNode( tempNode->sibling[ 0 ], ParseStatement() );
				Expect( TOKEN_SEMICOLON );
				SetNode( tempNode->sibling[ 1 ], ParsePlusExp() );
				Expect( TOKEN_SEMICOLON );
				SetNode( tempNode->sibling[ 2 ], ParseStatement() );
				Expect( TOKEN_RPAR );

				tempNode->child = SetData( ParseStatement() );

				return tempNode;

			case TOKEN_IDENTIFIER:
				{
				std::string name = lex.stringData;
				lex.Next();

				// Function.
				//
				if( lex.token == TOKEN_LPAR )
				{
					// Determine if this function has a registered name.
					//
					int index = NativeFunction::Find( nativeFunc, name );
					if( index != -1 )
					{
						// This is a native function call.
						//
						Expect( TOKEN_LPAR );
						tempNode = new ScriptNode( TOKEN_NATIVE_FUNC, ParseFuncValues(), SetData( new Fixed16( index )));
						return new ScriptNode( TOKEN_VOID, NULL, SetData( tempNode ));
					}

					// Add this function to the temporary map.
					//
					FuncMap::iterator it0 = funcs.find( name );
					if( it0 == funcs.end() )
					{
						CreateFuncVars( name );
					}

					tempNode = new ScriptNode( TOKEN_FUNCTION, NULL, SetData( name.c_str() ));

					tempNode->sibling = new ScriptNode[ 2 ];	// vars/statement

					Expect( TOKEN_LPAR );

					// Check if this function has no parameters.
					//
					if( lex.token == TOKEN_RPAR )
					{
						Expect( TOKEN_RPAR );

						// Function declaration.
						//
						if( lex.token == TOKEN_LCURLY )
						{
							SetNode( tempNode->sibling[ 1 ], ParseStatement() );

							currVars = currVars->parent;

							return tempNode;
						}
						// Function CALL.
						//
						else
						{
							currNode = new ScriptNode( TOKEN_VOID, NULL, SetData( tempNode ));

							return currNode;
						}
					}

					// Function declaration.
					//
					if( lex.token == TOKEN_IDENTIFIER && FindVar( lex.stringData, &globals ) == -1 && FindVar( lex.stringData, currVars ) == -1 )
					{
						// Ensure this function is not being declared within another function.
						//
						if( currVars->parent != NULL )
						{
							REPORT_PARSER_ERROR( lex.linenumber, "SYNTAX_ERROR", "Function '%s' cannot be declared within a function.", it0->first.c_str() )
						}

						// Ensure this function is not being redeclared.
						//
						if( it0 != funcs.end() )
						{
							REPORT_PARSER_ERROR( lex.linenumber, "SYNTAX_ERROR", "Function '%s' already defined.", it0->first.c_str() )
						}

						// Create function parameters.
						//
						SetNode( tempNode->sibling[ 0 ], ParseFuncParams() );

						Expect( TOKEN_RPAR );

						// Ensure the next token is a LCURLY,
						// or it is not a proper function declaration.
						//
						if( lex.token != TOKEN_LCURLY )
						{
							REPORT_PARSER_ERROR( lex.linenumber, "SYNTAX_ERROR", "Expected '{' for function declaration." );
						}

						// This is the function definition.
						//
						SetNode( tempNode->sibling[ 1 ], ParseStatement() );

						currVars = currVars->parent;

						return tempNode;
					}
					// Function CALL.
					//
					else
					{
						SetNode( tempNode->sibling[ 0 ], ParseFuncValues() );
						currNode = new ScriptNode( TOKEN_VOID, NULL, SetData( tempNode ));

						return currNode;
					}
				}
				// Variable.
				//
				else
				{
					// Add this variable to the map.
					//
					int isGlobal = FindVar( name, &globals );
					if( isGlobal == -1 && currVars == NULL )
					{
						globals.vars.push_back( name );
					}
					else
					if( FindVar( name, currVars ) == -1 && currVars != NULL && isGlobal == -1 )
					{
						currVars->vars.push_back( name );
					}
				
					tempNode = ParseVariable( name );
				
					token = lex.token;
					if( token == TOKEN_ASSIGN || token == TOKEN_ADD_ASSIGN || token == TOKEN_SUB_ASSIGN || token == TOKEN_MUL_ASSIGN  || token == TOKEN_DIV_ASSIGN )
					{
						lex.Next();

						currNode = new ScriptNode( token, ParsePlusExp(), SetData( tempNode ));
					}
					else
					{
						REPORT_PARSER_ERROR( lex.linenumber, "SYNTAX_ERROR", "Expected Assignment Operation." );
					}
					return currNode;
				}
				}

			case TOKEN_RETURN:
				lex.Next();
				return new ScriptNode( TOKEN_RETURN, ParsePlusExp() );
			
			case TOKEN_LCURLY:
				{
				// Create a variable container.
				//
				CreateFuncVars();

				std::vector< ScriptNode* > sNodes;
				tempNode = new ScriptNode( lex.token );

				lex.Next();
				while( lex.token != TOKEN_RCURLY && !didError )
				{
					sNodes.push_back( ParseStatement() );
				}

				SetNode( tempNode, sNodes );

				Expect( TOKEN_RCURLY );

				currVars = currVars->parent;
				}
				return tempNode;

			case TOKEN_PRINT:
				lex.Next();
				return new ScriptNode( TOKEN_PRINT, ParsePlusExp() );

			case TOKEN_SLEEP:
				lex.Next();
				return new ScriptNode( TOKEN_SLEEP, ParsePlusExp() );

			case TOKEN_PAUSE:
				lex.Next();
				return new ScriptNode( TOKEN_PAUSE );

			default:
				return new ScriptNode( TOKEN_VOID, NULL, SetData( ParsePlusExp() ));
		};
	}

	ScriptNode* ScriptParser::ParsePlusExp()
	{
		ScriptNode* node = ParseMulExp();
		int token = lex.token;
		if( token == TOKEN_ADD || token == TOKEN_SUB || 
			token == TOKEN_AND || token == TOKEN_OR )
		{
			lex.Next();
			return new ScriptNode( token, node, SetData( ParsePlusExp() ));
		}
		return node;
	}

	ScriptNode* ScriptParser::ParseMulExp()
	{
		ScriptNode* node = ParseFactorExp();
		int token = lex.token;
		if( token == TOKEN_MUL || token == TOKEN_DIV ||
			token == TOKEN_EQ  || token == TOKEN_NOT_EQ || 
			token == TOKEN_GT  || token == TOKEN_GT_EQ  || 
			token == TOKEN_LT  || token == TOKEN_LT_EQ )
		{
			lex.Next();
			return new ScriptNode( token, node, SetData( ParseMulExp() ));
		}
		return node;
	}

	ScriptNode* ScriptParser::ParseFactorExp()
	{
		ScriptNode* node = NULL;
		
		switch( lex.token )
		{
			case TOKEN_IDENTIFIER:
				{
				// Determine if this is a function call or variable.
				//
				std::string name = lex.stringData;
				lex.Next();
				
				if( lex.token == TOKEN_LPAR )
				{
					Expect( TOKEN_LPAR );
					node = new ScriptNode( TOKEN_FUNC_CALL, NULL );

					// Determine if this function has a registered name.
					//
					ScriptNode* tempNode;
					int index = NativeFunction::Find( nativeFunc, name );
					if( index != -1 )
					{
						tempNode = new ScriptNode( TOKEN_NATIVE_FUNC, NULL, SetData( new Fixed16( index )));
					}
					else
					{
						tempNode = new ScriptNode( TOKEN_FUNCTION, NULL, SetData( name.c_str() ));
					}

					tempNode->sibling = new ScriptNode[ 2 ];
					SetNode( tempNode->sibling[ 0 ], ParseFuncValues() );
					node->child.child = tempNode;
				}
				else
				{
					// Check if this variable is in the map.
					//
					if( FindVar( name, &globals ) == -1 && FindVar( name, currVars ) == -1 )
					{
						REPORT_PARSER_ERROR( lex.linenumber, "SYNTAX_ERROR", "Variable '%s' used without declaration.", name.c_str() );
					}
					node = ParseVariable( name );
				}
				}
				break;

			case TOKEN_STRING:
				node = new ScriptNode( TOKEN_STRING, NULL, SetData( lex.stringData.c_str() ));
				lex.Next();
				break;

			case TOKEN_BOOL:
			case TOKEN_NUMBER:
				node = new ScriptNode( TOKEN_NUMBER, NULL, SetData( new Fixed16( lex.numberData )));
				lex.Next();
				break;

			case TOKEN_SUB:
				lex.Next();
				node = new ScriptNode( TOKEN_NUMBER, NULL, SetData( new Fixed16( -lex.numberData )));
				lex.Next();
				break;

			case TOKEN_NOT:
				lex.Next();
				node = new ScriptNode( TOKEN_NOT, ParseFactorExp() );
				break;
				
			case TOKEN_LPAR:
				lex.Next();
				node = ParsePlusExp();
				Expect( TOKEN_RPAR );
				break;

			case TOKEN_LBRACKET:
				node = ParseVector();
				break;

			default:
				REPORT_PARSER_ERROR( lex.linenumber, "SYNTAX_ERROR", "Expected Expression" );
		};

		return node;
	}

	ScriptNode* ScriptParser::ParseVariable( std::string name )
	{
		ScriptNode* node = new ScriptNode( TOKEN_VAR, NULL, SetData( name.c_str() ));

		ScriptNode* tempNode = node;
		while( lex.token == TOKEN_LBRACKET && !didError )
		{
			lex.Next();
			tempNode->sibling = new ScriptNode( TOKEN_VECTOR_ACCESS, NULL, SetData( ParsePlusExp() ));
			tempNode = tempNode->sibling;
			Expect( TOKEN_RBRACKET );
		}

		return node;
	}

	ScriptNode* ScriptParser::ParseVector()
	{
		Expect( TOKEN_LBRACKET );
		
		std::vector< ScriptNode* > sNodes;
		ScriptNode* node = new ScriptNode( TOKEN_VECTOR_LIST );

		while( lex.token != TOKEN_RBRACKET && !didError )
		{
			if( lex.token == TOKEN_LBRACKET )
			{
				sNodes.push_back( ParseVector() );
			}
			else
			{
				sNodes.push_back( ParsePlusExp() );
			}

			if( lex.token != TOKEN_RBRACKET )
			{
				Expect( TOKEN_COMMA );
			}
		}

		SetNode( node, sNodes );

		Expect( TOKEN_RBRACKET );

		return node;
	}

	ScriptNode* ScriptParser::ParseFuncParams_INTERNAL()
	{
		if( lex.token == TOKEN_IDENTIFIER )
		{
			// Add this variable to the function's map.
			//
			if( FindVar( lex.stringData, &globals ) == -1 && FindVar( lex.stringData, currVars ) == -1 )
			{
				currVars->vars.push_back( lex.stringData );
				return new ScriptNode( TOKEN_VAR, NULL, SetData( lex.stringData.c_str() ));
			}
			else
			{
				REPORT_PARSER_ERROR( lex.linenumber, "SYNTAX_ERROR", "Variable '%s' has already been declared.", lex.stringData.c_str() );
				return NULL;
			}
		}
		else
		{
			REPORT_PARSER_ERROR( lex.linenumber, "SYNTAX_ERROR", "Expected Variable" );
			return NULL;
		}
	}

	ScriptNode* ScriptParser::ParseFuncParams()
	{
		ScriptNode* node = NULL;
		if( lex.token != TOKEN_RPAR )
		{
			node = ParseFuncParams_INTERNAL();
			lex.Next();
		}

		ScriptNode* tempNode = node;
		while( lex.token != TOKEN_RPAR && !didError )
		{
			Expect( TOKEN_COMMA );

			node->sibling = ParseFuncParams_INTERNAL();
			node = node->sibling;

			lex.Next();
		}

		return tempNode;
	}

	ScriptNode* ScriptParser::ParseFuncValues()
	{
		ScriptNode* node = NULL;
		if( lex.token != TOKEN_RPAR )
		{
			node = new ScriptNode( TOKEN_FUNC_CALL, NULL, SetData( ParsePlusExp() ));
		}

		ScriptNode* tempNode = node;
		while( node->sibling != NULL )
		{
			node = node->sibling;
		}

		while( lex.token != TOKEN_RPAR && !didError )
		{
			Expect( TOKEN_COMMA );

			node->sibling = new ScriptNode( TOKEN_FUNC_CALL, NULL, SetData( ParsePlusExp() ));
			node = node->sibling;
		}

		Expect( TOKEN_RPAR );
		
		return tempNode;
	}

	void ScriptParser::SetNode( ScriptNode& node, ScriptNode* node_ptr )
	{
		if( node_ptr != NULL )
		{
			node = ScriptNode( node_ptr->type, node_ptr->sibling, node_ptr->child );
			delete node_ptr;
		}
	}

	void ScriptParser::SetNode( ScriptNode* node, std::vector< ScriptNode* >& node_vec )
	{
		int count = (int)node_vec.size();
		if( count > 0 )
		{
			node->sibling = new ScriptNode[ count ];
			node->child = SetData( new Fixed16( count ));

			for( int x = 0; x < count; ++x )
			{
				node->sibling[ x ] = ScriptNode( node_vec[ x ]->type, node_vec[ x ]->sibling, node_vec[ x ]->child );
				delete node_vec[ x ];
			}
		}
	}

	int ScriptParser::FindVar( std::string name, FuncVars* vars )
	{
		if( vars != NULL )
		{
			int count = (int)vars->vars.size();
			for( int x = 0; x < count; ++x )
			{
				if( name.compare( vars->vars[ x ] ) == 0 )
				{
					return x;
				}
			}

			return FindVar( name, vars->parent );
		}

		return -1;
	}

	int ScriptParser::GetVarOffset( std::string name, FuncVars* vars )
	{
		_ASSERT( vars != NULL );

		while( vars != NULL )
		{
			int count = (int)vars->vars.size();
			for( int x = 0; x < count; ++x )
			{
				if( name.compare( vars->vars[ x ] ) == 0 )
				{
					// Function declaration.
					//
					if( vars->parent == NULL )
					{
						return -2 - x;
					}

					// Statement.
					//
					vars = vars->parent;
					while( vars->parent != NULL )
					{
						x += vars->vars.size();
						vars = vars->parent;
					}
					return x + 1;
				}
			}
			vars = vars->parent;
		}

		return 0;
	}

	// Create a variable container.
	//
	void ScriptParser::CreateFuncVars( std::string name )
	{
		funcs.insert( FuncPair( name, FuncVars() ));
		FuncMap::iterator it0 = funcs.find( name );
		it0->second.parent = currVars;
		currVars = &it0->second;
	}

	void ScriptParser::CreateFuncVars()
	{
		char buf[ 256 ];
		sprintf_s( buf, "~%d", funcs.size() );
		CreateFuncVars( buf );
	}
}