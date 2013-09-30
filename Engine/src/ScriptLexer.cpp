// Reads a script from a file, and performs functions.
//
#include <string>
#include <list>
#include <iostream>

#include "Util.h"
#include "Script.h"
#include "Archive.h"

namespace Sentinel
{
	#define SET_TOKEN( tokenNum, err )\
	{\
		token = tokenNum;\
		if( token == TOKEN_ERROR )\
		{\
			REPORT_SCRIPT_ERROR( linenumber, #tokenNum, err );\
		}\
		return 1;\
	}

	#define SET_STRING( tokenNum, str )\
		if( stringData == str )\
		{\
			SET_TOKEN( tokenNum, str )\
		}

	#define SET_STRING_NUMBER( tokenNum, str, num )\
		if( stringData == str )\
		{\
			numberData = num;\
			SET_TOKEN( tokenNum, str )\
		}

	#define READ_DATA( a )\
	{\
		while( a )\
		{\
			stringData.push_back( c );\
			c = *data++;\
		}\
	}

	ScriptLexer::ScriptLexer()
	{
		Startup();
	}

	ScriptLexer::ScriptLexer( const char* filename )
	{
		Startup();
		Load( filename );
	}

	int ScriptLexer::Load( const char* filename )
	{
		int i = Archive::ToBuffer( filename, data );
		copy = data;
		return i;
	}

	void ScriptLexer::Shutdown()
	{
		if( copy )
		{
			free( copy );
		}
		copy = NULL;
	}

	void ScriptLexer::Startup()
	{
		linenumber = 1;
		copy = NULL;
		data = NULL;
	}

	int ScriptLexer::Next()
	{
		numberData = 0;
		stringData.clear();

		for(;;)
		switch( c = *data++ )
		{
			// End of File.
			//
			case 0:
				SET_TOKEN( TOKEN_EOF, "" );

			// New line.
			//
			case '\n':
				++linenumber;
				break;

			// Space.
			//
			case ' ':
			case '\t':
				break;

			// Comment.
			//
			case '#':
				for( ; c != '\n' && c != 0; c = *data++ );
				data--;
				continue;
				
			// String.
			//
			case '\"':
				c = *data++;
				READ_DATA( c != '\"' && c != 0 && c != '\n' );
				if( c == 0 || c == '\n' )
				{
					--data;
					SET_TOKEN( TOKEN_ERROR, "String did not end with '\"'" );
				}
				SET_TOKEN( TOKEN_STRING, stringData );

			// Left Parentheses.
			//
			case '(':
				SET_TOKEN( TOKEN_LPAR, "(" );

			// Right Parentheses.
			//
			case ')':
				SET_TOKEN( TOKEN_RPAR, ")" );

			// Left Braket.
			//
			case '[':
				SET_TOKEN( TOKEN_LBRACKET, "[" );

			// Right Bracket.
			//
			case ']':
				SET_TOKEN( TOKEN_RBRACKET, "]" );

			// Left Curly.
			//
			case '{':
				SET_TOKEN( TOKEN_LCURLY, "{" );

			// Right Curly.
			//
			case '}':
				SET_TOKEN( TOKEN_RCURLY, "}" );

			// Equals.
			//
			case '=':
				if( *data == '=' )
				{
					++data;
					SET_TOKEN( TOKEN_EQ, "==" );
				}
				SET_TOKEN( TOKEN_ASSIGN, "=" );
				
			// Add.
			//
			case '+':
				if( *data == '=' )
				{
					++data;
					SET_TOKEN( TOKEN_ADD_ASSIGN, "+=" );
				}
				/*if( *data == '+' )
				{
					++data;
					SET_TOKEN( TOKEN_INCREMENT, "++" );
				}*/
				SET_TOKEN( TOKEN_ADD, "+" );
				
			// Subtract.
			//
			case '-':
				if( *data == '=' )
				{
					++data;
					SET_TOKEN( TOKEN_SUB_ASSIGN, "-=" );
				}
				/*if( *data == '-' )
				{
					++data;
					SET_TOKEN( TOKEN_DECREMENT, "--" );
				}*/
				SET_TOKEN( TOKEN_SUB, "-" );
				
			// Multiply.
			//
			case '*':
				if( *data == '=' )
				{
					++data;
					SET_TOKEN( TOKEN_MUL_ASSIGN, "*=" );
				}
				SET_TOKEN( TOKEN_MUL, "*" );
				
			// Divide.
			//
			case '/':
				if( *data == '=' )
				{
					++data;
					SET_TOKEN( TOKEN_DIV_ASSIGN, "/=" );
				}
				SET_TOKEN( TOKEN_DIV, "/" );
				
			// Or.
			//
			case '|':
				if( *data == '|' )
				{
					++data;
					SET_TOKEN( TOKEN_OR, "OR" );
				}
				SET_TOKEN( TOKEN_ERROR, "Invalid operator '|'" );
				
			// And.
			//
			case '&':
				if( *data == '&' )
				{
					++data;
					SET_TOKEN( TOKEN_AND, "AND" );
				}
				SET_TOKEN( TOKEN_ERROR, "Invalid operator '&'" );

			// Exclamation Point.
			//
			case '!':
				if( *data == '=' )
				{
					++data;
					SET_TOKEN( TOKEN_NOT_EQ, "!=" );
				}
				SET_TOKEN( TOKEN_NOT, "!" );
				
			// Greater Than.
			//
			case '>':
				if( *data == '=' )
				{
					++data;
					SET_TOKEN( TOKEN_GT_EQ, ">=" );
				}
				SET_TOKEN( TOKEN_GT, ">" );
				
			// Greater Than.
			//
			case '<':
				if( *data == '=' )
				{
					++data;
					SET_TOKEN( TOKEN_LT_EQ, "<=" );
				}
				SET_TOKEN( TOKEN_LT, "<" );
				
			// Comma.
			//
			case ',':
				SET_TOKEN( TOKEN_COMMA, "," );

			// Semi-Colon.
			//
			case ';':
				SET_TOKEN( TOKEN_SEMICOLON, ";" );

			// Characters and Numbers.
			//
			default:
				if( isalpha( c ))
				{
					READ_DATA( isalpha( c ) || c == '_' || isdigit(c) );
					data--;

					// Check for keywords.
					//
					SET_STRING( TOKEN_IF, "if" )
					else
					SET_STRING( TOKEN_ELSE, "else" )
					else
					SET_STRING( TOKEN_WHILE, "while" )
					else
					SET_STRING( TOKEN_FOR, "for" )
					else
					SET_STRING( TOKEN_RETURN, "return" )
					else
					SET_STRING_NUMBER( TOKEN_NUMBER, "null", 0 )
					else
					SET_STRING_NUMBER( TOKEN_BOOL, "true", 1 )
					else
					SET_STRING_NUMBER( TOKEN_BOOL, "false", 0 )
					else
					SET_STRING( TOKEN_PRINT, "print" )
					else
					SET_STRING( TOKEN_SLEEP, "sleep" )
					else
					SET_STRING( TOKEN_PAUSE, "pause" )
					
					SET_TOKEN( TOKEN_IDENTIFIER, stringData )
				}
				else
				if( isdigit( c ) || c =='.' )
				{
					READ_DATA( isdigit( c ));
					if( c == '.' )
					{
						stringData.push_back( c );
						c = *data++;
						READ_DATA( isdigit( c ));
					}
					numberData = (float)atof( stringData.c_str() );
					data--;

					SET_TOKEN( TOKEN_NUMBER, "" );
				}
				else
				if( c != 13 )	// carriage return
				{
					if( c == '.' )
					{
						SET_TOKEN( TOKEN_ERROR, "Cannot start number with '.'" );
					}

					char buf[ 256 ];
					sprintf_s( buf, "Unknown Token '%c'", c );
					SET_TOKEN( TOKEN_ERROR, buf );
				}
		}
	}
}