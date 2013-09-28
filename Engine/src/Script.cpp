// Custom scripting language.
//
#include "Script.h"
#include "RandomValue.h"

namespace Sentinel
{
	#define POP( type )\
		type->back().Release();\
		type->pop_back();

	#define POP_COUNT( a )\
		for( UINT x = 0; x < a; ++x )\
		{\
			POP( stackCopy )\
		}

	#define GET_NUMBER( a )\
	{\
		UCHAR c[ 4 ];\
		for( UINT y = 0; y < 4; ++y )\
		{\
			c[ y ] = comp.data[ ++IP ];\
		}\
		BYTEStoNUM( c, a );\
	}

	#define CHECK_VEC( a )\
		GET_NUMBER( loc0 );\
		v0 = &a;\
		while( vecA.size() > 0 )\
		{\
			if( v0->type != VALUE_VECTOR )\
			{\
				throw( ScriptException( IP, "RUN-TIME_ERROR", "Invalid Vector Access." ));\
			}\
			if( vecA.back() >= (int)v0->vec->data.size() || vecA.back() < 0 )\
			{\
				throw( ScriptException( IP, "RUN-TIME_ERROR", "Vector Access out of range." ));\
			}\
			v0 = &v0->vec->data[ vecA.back() ];\
			vecA.pop_back();\
		}

	#define PUSH_VAR( a )\
	{\
		CHECK_VEC( a )\
		if( v0->type == VALUE_NUMBER )\
		{\
			stackCopy->push_back( Value( *v0->num ));\
			break;\
		}\
		else\
		if( v0->type == VALUE_STRING )\
		{\
			STRING_ALLOC()\
			v0->str->incRef();\
		}\
		else\
		if( v0->type == VALUE_VECTOR )\
		{\
			VECTOR_ALLOC()\
			v0->vec->incRef();\
		}\
		stackCopy->push_back( *v0 );\
	}\
	break;
	
	#define GET_VALUES()\
		loc0 = (int)stackCopy->size()-1;\
		v0 = &(*stackCopy)[ loc0 ];\
		v1 = &(*stackCopy)[ loc0 - 1 ];

	#define OP_VALUES( type )\
		POP( type )\
		POP( type )\
		type->push_back( v );\
		break;

	#define MATH_OP( oper )\
	{\
		GET_VALUES()\
		if( v0->type != VALUE_NUMBER || v1->type != VALUE_NUMBER )\
		{\
			throw( ScriptException( 0, "RUN-TIME_ERROR", "Operation not allowed." ));\
		}\
		v = Value( Fixed16( v1->num->data oper v0->num->data ));\
		OP_VALUES( stackCopy )\
	}

	#define BOOL_OP( oper )\
	{\
		GET_VALUES()\
		if( v0->type != VALUE_NUMBER || v1->type != VALUE_NUMBER )\
		{\
			if( v0->type != v1->type )\
			{\
				THROW_SCRIPT_EXCEPTION( 0, "RUN-TIME_ERROR", "Boolean operation '%s' not allowed on String to Vector.", #oper )\
			}\
			else\
			if( v0->type == VALUE_VECTOR )\
			{\
				THROW_SCRIPT_EXCEPTION( 0, "RUN-TIME_ERROR", "Boolean operation '%s' not allowed on Vector.", #oper )\
			}\
		}\
		v = Value( Fixed16( (v1->num->data oper v0->num->data) ? 1 : 0 ));\
		OP_VALUES( stackCopy )\
	}

	#define STORE( a )\
	{\
		CHECK_VEC( a )\
		if( v0->type > VALUE_VECTOR )\
		{\
			throw( ScriptException( 0, "RUN-TIME_ERROR", "Assign to invalid type." ));\
		}\
		v0->Release();\
		v0->Assign( stackCopy->back() );\
		POP( stackCopy )\
	}\
	break;

	// Native Global FUNCTIONS.
	//
	Value RandValueInt( const Value& minValue, const Value& maxValue )
	{
		return Value( Fixed16( RandomValue( (int)minValue.num->data, (int)maxValue.num->data )));
	}

	Value RandValueFloat( const Value& minValue, const Value& maxValue )
	{
		return Value( Fixed16( RandomValue( minValue.num->data, maxValue.num->data )));
	}

	Value SinValue( const Value& X )
	{
		return Value( Fixed16( sin( X.num->data )));
	}

	Value CosValue( const Value& X )
	{
		return Value( Fixed16( cos( X.num->data )));
	}

	Value SqrtValue( const Value& X )
	{
		return Value( Fixed16( sqrt( X.num->data )));
	}

	//////////////////////////////////////////////////////////

	Script::Script()
	{
		Startup();
	}

	Script::Script( const char* filename )
	{
		Startup();
		Load( filename );
	}

	Script::~Script()
	{
		for( UINT x = 0; x < global.size(); ++x )
		{
			global[ x ].Release();
		}
		
		for( UINT x = 0; x < stack.size(); ++x )
		{
			stack[ x ].Release();
		}
	}

	// Zero out.
	//
	void Script::Startup()
	{
		m_numGlobals = 0;
		
		RegisterVar( "deltaTime", Value( 0.0f ));
		
		m_waitTime = 0;
		IP = 0;
		FP = 0;
	}

	// Load the script into memory.
	//
	int Script::Load( const char* filename )
	{
		RegisterFunc( "RandInt",	(FuncPtr2)RandValueInt,		"NN" );
		RegisterFunc( "RandFloat",	(FuncPtr2)RandValueFloat,	"NN" );
		RegisterFunc( "sin",		(FuncPtr1)SinValue,			"N" );
		RegisterFunc( "cos",		(FuncPtr1)CosValue,			"N" );
		RegisterFunc( "sqrt",		(FuncPtr1)SqrtValue,		"N" );
		
		TRACE( "Loading Script: " << filename );

		int i = comp.Load( filename );
		
		if( i != 0 )
		{
			TRACE( "Success!" );
		}
		else
		{
			TRACE( "Failed!" );
		}

		return i;
	}

	// Register a global variable for this script.
	//
	void Script::RegisterVar( const std::string& name, const Value& _value )
	{
		int isGlobal = comp.parser.FindVar( name, &comp.parser.globals );
		if( isGlobal == -1 )
		{
			comp.parser.globals.vars.push_back( name );
			global.push_back( _value );
			++m_numGlobals;
		}
		else
		{
			global[ isGlobal ] = _value;
		}
	}

	// Register a native C++ function call.
	// Allows functions to be replaced.
	//
	#define REGISTER_FUNC_HELPER_CPP( count )\
	void Script::RegisterFunc( const std::string& name, FuncPtr##count func, const std::string& args )\
	{\
		int index = NativeFunction::Find( comp.parser.nativeFunc, name, (int)args.size() );\
		if( index == -1 )\
		{\
			comp.parser.nativeFunc.push_back( NativeFunction( name, func, args ));\
		}\
		else\
		{\
			comp.parser.nativeFunc[ index ].Func##count = func;\
			comp.parser.nativeFunc[ index ].args = args;\
		}\
	}

	REGISTER_FUNC_HELPER_CPP( 0 )
	REGISTER_FUNC_HELPER_CPP( 1 )
	REGISTER_FUNC_HELPER_CPP( 2 )
	REGISTER_FUNC_HELPER_CPP( 3 )
	REGISTER_FUNC_HELPER_CPP( 4 )

	// Release node memory.
	//
	void Script::Shutdown()
	{
		for( UINT x = 0; x < m_numGlobals; ++x )
		{
			global[ x ].Release();
		}
		
		comp.Shutdown();

		OUTPUT_ALLOCS()
	}

	// Output debug data.
	//
	void Script::Output( std::fstream& file )
	{
		comp.Output( file );
	}

	// Output debug data to a file.
	//
	void Script::Output( char* filename )
	{
		std::fstream file( filename, std::ios::out );

		if( !file.is_open() )
		{
			printf( "Failed to open for Output.\n" );
		}
		else
		{
			Output( file );
		}

		file.close();
	}

	// Timer used for sleep.
	// It is important to save the time, and reset it to ensure
	// that multiple calls for execution without the use of SCRIPT_CONTINUE
	// results in the correct behavior.
	// This is not necessary if only one object is using this script.
	//
	void Script::SetWaitTime( float _waitTime )
	{
		m_waitTime = _waitTime;
	}

	float Script::WaitTime()
	{
		return m_waitTime;
	}

	UINT Script::NumGlobals()
	{
		return m_numGlobals;
	}

	// Copy the values within this stack.
	//
	#define COPY_STACK( type )\
		if( type##Copy->size() < type.size() )\
		{\
			type##Copy->resize( type.size() );\
			for( UINT x = 0; x < type.size(); ++x )\
			{\
				((*type##Copy)[ x ]) = Value( type[ x ] );\
			}\
		}

	void Script::CopyStack( std::vector< Value >* globalCopy, std::vector< Value >* stackCopy )
	{
		COPY_STACK( global )
		COPY_STACK( stack )
	}

	// Override the current stack.
	//
	#define OVERRIDE_STACK( type )\
		type.resize( type##Copy->size() );\
		for( UINT x = 0; x < type##Copy->size(); ++x )\
		{\
			type[ x ] = type##Copy->at( x );\
		}

	void Script::SetStack( std::vector< Value >* globalCopy, std::vector< Value >* stackCopy )
	{
		OVERRIDE_STACK( global )
		OVERRIDE_STACK( stack )
	}

	// Print a value.
	//
	void Script::PrintValue( const Value& v )
	{
		switch( v.type )
		{
			case VALUE_NUMBER:
				TRACE_NE( v.num->data );
				break;

			case VALUE_STRING:
				TRACE_NE( v.str->data.c_str() );
				break;

			case VALUE_VECTOR:
				{
				TRACE_NE( "[ " );
				int count = v.vec->data.size()-1;
				for( int x = 0; x <= count; ++x )
				{
					PrintValue( v.vec->data[ x ] );

					if( x < count )
					{
						TRACE_NE( ", " );
					}
				}
				TRACE_NE( " ]" );
				}
				break;
		}
	}

	// Execute the code starting with a specific instruction.
	// This is risky as the wrong IP will result in unexpected results.
	// Returns next instruction location to be executed; SCRIPT_END = end of script
	//
	int Script::Execute( float deltaTime, int _IP, std::vector< Value >* globalCopy, std::vector< Value >* stackCopy )
	{
		// Prevent byte code from executing if it has finished.
		// Fail safe for per frame executions.
		//
		if( _IP == SCRIPT_END )
		{
			return SCRIPT_END;
		}

		try
		{
			// Begin interpreting byte code.
			//
			int lastByte = (int)comp.data.size();

			int loc0 = 0;
			int loc1 = 0;
			UINT count = 0;

			Fixed16 num;
			Value* v0;
			Value* v1;
			Value  v;
			Value RV;

			// Implies SCRIPT_CONTINUE
			//
			if( _IP >= 0 )
			{
				IP = _IP;
			}

			if( globalCopy == NULL )
			{
				globalCopy = &global;
			}

			if( stackCopy == NULL )
			{
				stackCopy = &stack;
			}

			((*globalCopy)[ GLOBAL_DELTATIME ]).num->data = Fixed16( deltaTime );
			
			while( IP < lastByte )
			{
				switch( (int)comp.data[ IP ] )
				{
					case OP_PUSH_NUM:
						{
						GET_NUMBER( num.value );
						stackCopy->push_back( Value( num ));
						}
						break;

					case OP_PUSH_STR:
						stackCopy->push_back( Value( (const char*)&comp.data[ ++IP ] ));
						IP += (int)strlen( (const char*)&comp.data[ IP ] );
						break;

					case OP_PUSH_VAR:
						PUSH_VAR( (*stackCopy)[ FP + loc0 ] )

					case OP_PUSH_VAR_G:
						PUSH_VAR( (*globalCopy)[ loc0 ] )
						
					case OP_PUSH_VEC:
						{
						GET_NUMBER( num.value )
						count = (int)num;

						v.type = VALUE_VECTOR;
						v.vec = new RefObject< std::vector< Value >>();
						v.vec->data.resize( count );

						for( int x = count-1; x > -1; --x )
						{
							v.vec->data[ x ].Assign( stackCopy->back() );
							POP( stackCopy )
						}

						stackCopy->push_back( v );

						VECTOR_ALLOC()
						}
						break;

					case OP_VEC_A:
						{
						vecA.push_back( (int)stackCopy->back().num->data );
						POP( stackCopy );
						}
						break;

					case OP_ADD:
						{
						GET_VALUES()
					
						if( v0->type != v1->type || v0->type >= VALUE_VECTOR || v1->type >= VALUE_VECTOR )
						{
							throw( ScriptException( IP, "RUN-TIME_ERROR", "Operation not allowed." ));
						}

						v.type = v0->type;
						if( v0->type == VALUE_NUMBER )
						{
							v.num = new RefObject< Fixed16 >( v0->num->data + v1->num->data );
						}
						else
						{
							v.str = new RefObject< std::string >( v1->str->data + v0->str->data );
							STRING_ALLOC()
						}
						OP_VALUES( stackCopy )
						}

					case OP_SUB:
						MATH_OP( - )
					
					case OP_MUL:
						MATH_OP( * )
					
					case OP_DIV:
						MATH_OP( / )
					
					case OP_STORE:
						{
						GET_NUMBER( loc0 );\
						v0 = &(*stackCopy)[ FP + loc0 ];\
						while( vecA.size() > 0 )\
						{\
							if( v0->type != VALUE_VECTOR )\
							{\
								throw( ScriptException( IP, "RUN-TIME_ERROR", "Invalid Vector Access." ));\
							}\
							if( vecA.back() >= (int)v0->vec->data.size() || vecA.back() < 0 )\
							{\
								throw( ScriptException( IP, "RUN-TIME_ERROR", "Vector Access out of range." ));\
							}\
							v0 = &v0->vec->data[ vecA.back() ];\
							vecA.pop_back();\
						}
						if( v0->type >= VALUE_VECTOR )\
						{\
							throw( ScriptException( 0, "RUN-TIME_ERROR", "Assign to invalid type." ));\
						}\
						v0->Release();\
						v0->Assign( stackCopy->back() );\
						POP( stackCopy )\
						}
						//STORE( stack[ FP + loc0 ] )
						break;

					case OP_STORE_G:
						STORE( (*globalCopy)[ loc0 ] )
						
					case OP_INCREMENT:
						break;

					case OP_DECREMENT:
						break;

					case OP_OR:
						BOOL_OP( || )

					case OP_AND:
						BOOL_OP( && )

					case OP_NOT:
						if( stackCopy->back().type != VALUE_NUMBER )
						{
							throw( ScriptException( IP, "RUN-TIME_ERROR", "Operation NOT (!) used on non-numeric value." ));
						}
						stackCopy->back() = Value( (stackCopy->back().num != 0) ? 0 : 1 );
						break;

					case OP_EQ:
						BOOL_OP( == )

					case OP_NOT_EQ:
						BOOL_OP( != )

					case OP_GT:
						BOOL_OP( > )

					case OP_GT_EQ:
						BOOL_OP( >= )

					case OP_LT:
						BOOL_OP( < )

					case OP_LT_EQ:
						BOOL_OP( <= )

					case OP_PUSH_ADDR:
						{
						GET_NUMBER( count )
						stackCopy->resize( stackCopy->size() + count );
						}
						break;

					case OP_PUSH_ADDR_G:
						{
						GET_NUMBER( count )
						if( (int)globalCopy->size() < count )
						{
							globalCopy->resize( count );
						}
						}
						break;

					case OP_POP:
						{
						GET_NUMBER( count )
						POP_COUNT( count )
						}
						break;

					case OP_IFNE:
						{
						GET_NUMBER( count )

						if( stackCopy->back().num->data == 0 )
						{
							IP = count;
						}

						POP( stackCopy )
						}
						break;
						
					case OP_JUMP:
						GET_NUMBER( IP )
						break;

					case OP_CALL:
						{
						loc0 = IP + 8;
						GET_NUMBER( loc1 )
						GET_NUMBER( count )

						funcPop.push_back( count );

						IP = loc1 - 1;
						stackCopy->push_back( Value() );
						Value& vTemp = stackCopy->back();
						vTemp.type = VALUE_RA;
						vTemp.RA   = loc0;

						loc1 = FP;
						FP = (int)stackCopy->size();
						stackCopy->push_back( Value() );
						vTemp = stackCopy->back();
						vTemp.type = VALUE_FP;
						vTemp.FP   = loc1;
						}
						break;

					case OP_RETURN:
						{
						Value& vTemp = stackCopy->back();
						RV.Assign( vTemp );

						GET_NUMBER( count )
						POP_COUNT( count + 1 )
						
						FP = vTemp.FP;
						POP( stackCopy )

						IP = vTemp.RA;
						POP( stackCopy )

						count = funcPop.back();
						POP_COUNT( count )
						funcPop.pop_back();

						stackCopy->push_back( RV );
						}
						break;

					case OP_PRINT:
						PrintValue( stackCopy->back() );
						TRACE( "" );
						POP( stackCopy )
						break;

					case OP_SLEEP:
						if( stackCopy->back().type != VALUE_NUMBER )
						{
							THROW_SCRIPT_EXCEPTION( IP, "RUN-TIME_ERROR", "Sleep not using number" )
						}

						// Wait for a number of seconds.
						// 0 = pass this frame
						//
						if( m_waitTime <= stackCopy->back().num->data )
						{
							m_waitTime += deltaTime;

							return IP;
						}

						m_waitTime = 0;
						
						POP( stackCopy )
						break;

					case OP_PAUSE:
						return ++IP;

					case OP_END:
						return SCRIPT_END;

					case OP_NATIVE_FUNC:
						{
						NativeFunction* func = &comp.parser.nativeFunc[ (UINT)comp.data[ ++IP ] ];
						
						UINT len = func->args.size();
						if( len > 4 )
						{
							THROW_SCRIPT_EXCEPTION( IP, "RUN-TIME_ERROR", "Invalid number of args( %d ) used in '%s'", len, func->name.c_str() )
						}

						std::vector< Value >::iterator it = stackCopy->end() - 1;

						UINT index = UINT_MAX;
						for( UINT x = 0; x < len; ++x )
						{
							switch( toupper( func->args[ x ] ))
							{
								case 'P':
									index = x;
								case 'N':
									if( (*(it-x)).type != VALUE_NUMBER )
									{
										THROW_SCRIPT_EXCEPTION( IP, "RUN-TIME_ERROR", "Invalid Number arg[%d] used in '%s'", x, func->name.c_str() )
									}
									break;

								case 'S':
									if( (*(it-x)).type != VALUE_STRING )
									{
										THROW_SCRIPT_EXCEPTION( IP, "RUN-TIME_ERROR", "Invalid String arg[%d] used in '%s'", x, func->name.c_str() )
									}
									break;

								case 'V':
									if( (*(it-x)).type != VALUE_VECTOR )
									{
										THROW_SCRIPT_EXCEPTION( IP, "RUN-TIME_ERROR", "Invalid Vector arg[%d] used in '%s'", x, func->name.c_str() )
									}
									break;

								default:
									THROW_SCRIPT_EXCEPTION( IP, "RUN-TIME_ERROR", "Invalid arg[%d] used in '%s'", x, func->name )
									break;
							}
						}

						switch( len )
						{
							case 0:
								RV = func->Func0();
								break;

							case 1:
								RV = func->Func1( *(it) );
								break;

							case 2:
								RV = func->Func2( *(it), *(it-1) );
								break;

							case 3:
								RV = func->Func3( *(it), *(it-1), *(it-2) );
								break;

							case 4:
								RV = func->Func4( *(it), *(it-1), *(it-2), *(it-3) );
								break;
						}

						for( UINT x = 0; x < len; ++x )
						{
							POP( stackCopy )
						}

						stackCopy->push_back( RV );
						}
						break;

					default:
						throw( ScriptException( 0, "SYNTAX_ERROR", "Unknown operation." ));	// should never reach here
				}

				// Next byte.
				//
				++IP;
			}
		}
		catch( ScriptException& e )
		{
			Shutdown();
			OutputDebugStringA( e.what() );
		}

		return SCRIPT_END;	// complete, should never reach here
	}

	///////////////////////////////////////////////////////////////////
	// Create the expected token as a string.
	//
	char* TokenToString( int token )
	{
		switch( token )
		{
			case TOKEN_VOID:
				return "VOID";

			case TOKEN_ERROR:
				return "ERROR";

			case TOKEN_EOF:
				return "EOF";

			case TOKEN_NONE:
				return "NONE";

			case TOKEN_NUMBER:
				return "NUM";

			case TOKEN_BOOL:
				return "BOOL";

			case TOKEN_STRING:
				return "STR";

			case TOKEN_VECTOR_ACCESS:
				return "VEC_A";

			case TOKEN_VECTOR_LIST:
				return "VEC_L";

			case TOKEN_FUNCTION:
				return "FUNC";

			case TOKEN_VAR:
				return "VAR";

			case TOKEN_ADD:
				return "'+'";

			case TOKEN_SUB:
				return "'-'";

			case TOKEN_MUL:
				return "'*'";

			case TOKEN_DIV:
				return "'/'";

			case TOKEN_ASSIGN:
				return "'='";

			case TOKEN_ADD_ASSIGN:
				return "'+='";

			case TOKEN_SUB_ASSIGN:
				return "'-='";

			case TOKEN_MUL_ASSIGN:
				return "'*='";

			case TOKEN_DIV_ASSIGN:
				return "'/='";

			case TOKEN_INCREMENT:
				return "'++'";

			case TOKEN_DECREMENT:
				return "'--'";

			case TOKEN_OR:
				return "'||'";

			case TOKEN_AND:
				return "'&&'";

			case TOKEN_NOT:
				return "'!'";

			case TOKEN_EQ:
				return "'=='";

			case TOKEN_NOT_EQ:
				return "'!='";

			case TOKEN_GT:
				return "'>'";

			case TOKEN_GT_EQ:
				return "'>='";

			case TOKEN_LT:
				return "'<'";

			case TOKEN_LT_EQ:
				return "'<='";

			case TOKEN_COMMENT:
				return "'#'";

			case TOKEN_IF:
				return "IF";

			case TOKEN_ELSE:
				return "ELSE";

			case TOKEN_WHILE:
				return "WHILE";

			case TOKEN_FOR:
				return "FOR";

			case TOKEN_RETURN:
				return "RETURN";

			case TOKEN_LPAR:
				return "'('";
				
			case TOKEN_RPAR:
				return "')'";

			case TOKEN_LBRACKET:
				return "'['";
				
			case TOKEN_RBRACKET:
				return "']'";

			case TOKEN_LCURLY:
				return "'{'";

			case TOKEN_RCURLY:
				return "'}'";

			case TOKEN_COMMA:
				return "','";

			case TOKEN_SEMICOLON:
				return "';'";

			case TOKEN_PRINT:
				return "PRINT";

			case TOKEN_SLEEP:
				return "SLEEP";

			case TOKEN_PAUSE:
				return "PAUSE";

			case TOKEN_NATIVE_FUNC:
				return "N_FUNC";

			default:
				return "Unknown Token";
		}
	}

	/////////////////////////////////////////////////
	// Values
	//
	Value::Value()
	{
		type = VALUE_UNDEFINED;
		num = NULL;
	}

	Value::Value( Fixed16 _num )
	{
		type = VALUE_NUMBER;
		num  = new RefObject< Fixed16 >( _num );
	}

	Value::Value( const char* _str )
	{
		type = VALUE_STRING;
		str = new RefObject< std::string >( _str );
		STRING_ALLOC()
	}

	void Value::Release()
	{
		if( type == VALUE_NUMBER )
		{
			if( num && num->decRef() == 0 )
			{
				delete num;
				num = NULL;
			}
		}
		else
		if( type == VALUE_STRING )
		{
			STRING_DEALLOC()
			if( str && str->decRef() == 0 )
			{
				delete str;
				str = NULL;
			}
		}
		else
		if( type == VALUE_VECTOR )
		{
			VECTOR_DEALLOC()
			if( vec && vec->decRef() == 0 )
			{
				int count = (int)vec->data.size();
				for( int x = 0; x < count; ++x )
				{
					vec->data[ x ].Release();
				}
				delete vec;
				vec = NULL;
			}
		}

		type = VALUE_UNDEFINED;
	}

	void Value::Assign( Value& right )
	{
		if( right.type == VALUE_NUMBER )
		{
			right.num->incRef();
		}
		else
		if( right.type == VALUE_STRING )
		{
			STRING_ALLOC()
			right.str->incRef();
		}
		else
		if( right.type == VALUE_VECTOR )
		{
			VECTOR_ALLOC()
			right.vec->incRef();
		}
		*this = right;
	}
}