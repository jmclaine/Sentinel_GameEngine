#pragma once

#include <map>
#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include <fstream>

#include "Debug.h"
#include "FixedPoint.h"
#include "RefCounter.h"

namespace Sentinel
{

#define NATIVE_NUM_FUNC1(type, globalFunc, classFunc, cast)\
		Value globalFunc(const Value& value)\
		{\
			type::Global->classFunc((cast)value.num->data);\
			return Value();\
		}

#define NATIVE_NUM_FUNC2(type, globalFunc, classFunc, cast)\
		Value globalFunc(const Value& X, const Value& Y)\
		{\
			type::Global->classFunc(vec2<cast>((cast)X.num->data, (cast)Y.num->data));\
			return Value();\
		}

#define NATIVE_NUM_FUNC3(type, globalFunc, classFunc, cast)\
		Value globalFunc(const Value& X, const Value& Y, const Value& Z)\
		{\
			type::Global->classFunc(vec3<cast>((cast)X.num->data, (cast)Y.num->data, (cast)Z.num->data));\
			return Value();\
		}

	////////////////////////////////////////////////////////////////////////////////////////

#define REPORT_SCRIPT_ERROR(linenumber, title, msg)\
		Debug::Log(STREAM(linenumber << "\t" << title << "\t" << msg));

#define OUTPUT_INFO(a, ...)\
		if (!file.is_open())\
		{\
			printf(a, __VA_ARGS__);\
		}\
		else\
		{\
			char buf[256];\
			memset(buf, 0, 256);\
			sprintf_s(buf, a, __VA_ARGS__);\
			file.write(buf, strlen(buf));\
		}

#define THROW_SCRIPT_EXCEPTION(linenumber, title, msg, ...)\
		char buf[256];\
		sprintf_s(buf, msg, __VA_ARGS__);\
		throw(ScriptException(linenumber, title, buf));

	////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	static int stringAlloc   = 0;
	static int stringDealloc = 0;
	static int vectorAlloc   = 0;
	static int vectorDealloc = 0;

#define STRING_ALLOC() ++stringAlloc;
#define STRING_DEALLOC() ++stringDealloc;
#define VECTOR_ALLOC() ++vectorAlloc;
#define VECTOR_DEALLOC() ++vectorDealloc;
#define OUTPUT_ALLOCS()\
	Debug::Log("Script Info:");\
	Debug::Log(STREAM("String Allocations: " << stringAlloc));\
	Debug::Log(STREAM("String Deallocations: " << stringDealloc));\
	Debug::Log(STREAM("Vector Allocations: " << vectorAlloc));\
	Debug::Log(STREAM("Vector Deallocations: " << vectorDealloc));
#else
#define STRING_ALLOC();
#define STRING_DEALLOC();
#define VECTOR_ALLOC();
#define VECTOR_DEALLOC();
#define OUTPUT_ALLOCS();
#endif

	enum ScriptExecution
	{
		SCRIPT_END = -2,
		SCRIPT_CONTINUE = -1,
	};

	enum TokenTypes
	{
		TOKEN_VOID = -4,	// function call may return a Value, but it is unused
		TOKEN_FUNC_CALL,		// function call returns Value
		TOKEN_ERROR,
		TOKEN_EOF,
		TOKEN_NONE,

		TOKEN_NUMBER,	// 1
		TOKEN_BOOL,
		TOKEN_STRING,
		TOKEN_VECTOR_ACCESS,
		TOKEN_VECTOR_LIST,
		TOKEN_FUNCTION,
		TOKEN_IDENTIFIER,
		TOKEN_VAR,
		TOKEN_ADD,
		TOKEN_SUB,
		TOKEN_MUL,
		TOKEN_DIV,
		TOKEN_ASSIGN,
		TOKEN_ADD_ASSIGN,
		TOKEN_SUB_ASSIGN,
		TOKEN_MUL_ASSIGN,
		TOKEN_DIV_ASSIGN,
		TOKEN_INCREMENT,
		TOKEN_DECREMENT,
		TOKEN_OR,
		TOKEN_AND,
		TOKEN_NOT,
		TOKEN_EQ,
		TOKEN_NOT_EQ,
		TOKEN_GT,
		TOKEN_GT_EQ,
		TOKEN_LT,
		TOKEN_LT_EQ,
		TOKEN_COMMENT,
		TOKEN_IF,
		TOKEN_THEN,
		TOKEN_ELSE,
		TOKEN_WHILE,
		TOKEN_FOR,
		TOKEN_RETURN,
		TOKEN_LPAR,
		TOKEN_RPAR,
		TOKEN_LBRACKET,
		TOKEN_RBRACKET,
		TOKEN_LCURLY,
		TOKEN_RCURLY,
		TOKEN_COMMA,
		TOKEN_SEMICOLON,
		TOKEN_PRINT,
		TOKEN_SLEEP,
		TOKEN_PAUSE,
		TOKEN_NATIVE_FUNC,
	};

	enum Operations
	{
		OP_PUSH_NUM,
		OP_PUSH_STR,
		OP_PUSH_VAR,
		OP_PUSH_VAR_G,
		OP_PUSH_VEC,
		OP_VEC_A,
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
		OP_STORE,
		OP_ADD_STORE,
		OP_SUB_STORE,
		OP_MUL_STORE,
		OP_DIV_STORE,
		OP_STORE_G,
		OP_INCREMENT,
		OP_DECREMENT,
		OP_OR,
		OP_AND,
		OP_NOT,
		OP_EQ,
		OP_NOT_EQ,
		OP_GT,
		OP_GT_EQ,
		OP_LT,
		OP_LT_EQ,
		OP_PUSH_ADDR,
		OP_PUSH_ADDR_G,
		OP_POP,
		OP_IFNE,
		OP_JUMP,
		OP_CALL,
		OP_RETURN,
		OP_PRINT,
		OP_SLEEP,
		OP_PAUSE,
		OP_RAND_INT,
		OP_RAND_FLT,
		OP_NATIVE_FUNC,
		OP_END,
	};

	enum ValueTypes
	{
		VALUE_UNDEFINED,
		VALUE_NUMBER,
		VALUE_STRING,
		VALUE_VECTOR,
		VALUE_FP,
		VALUE_RA,
	};

	enum NativeGlobals
	{
		GLOBAL_DELTATIME,
	};

	char* TokenToString(int token);

	////////////////////////////////////////////////////////////////////////////////////////
	// Storage for each token for processing.
	//
	struct ScriptNode
	{
		int type;
		ScriptNode* sibling;

		union ChildData
		{
			ScriptNode* child;
			Fixed16* numberData;
			char* stringData;
		};
		ChildData child;

		ScriptNode();
		ScriptNode(int type);
		ScriptNode(int type, ScriptNode* sibling);
		ScriptNode(int type, ScriptNode* sibling, ChildData child);

		void Shutdown();

		void Output(std::fstream& file, int width = 0);
	};

	union ScriptNode::ChildData SetData(ScriptNode* child);
	union ScriptNode::ChildData SetData(Fixed16* child);
	union ScriptNode::ChildData SetData(const char* child);


	////////////////////////////////////////////////////////////////////////////////////////
	// Storage for each Value.
	// Uses a reference counter.
	//
	struct Value
	{
		int type;
		union
		{
			RefObject<Fixed16>* num;
			RefObject<std::string>* str;
			RefObject<std::vector<Value>>* vec;

			int FP;	// frame (stack) pointer
			int RA;	// return address
		};

		//////////////////////////////////////

		Value();
		Value(Fixed16 _num);
		Value(const char* _str);

		void Release();

		void Assign(Value& right);
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// Variables and Functions by location.
	// Name, Data Location
	//
	struct FuncVars
	{
		std::vector< std::string > vars;
		int loc;

		// Used to chain upwards for storage counter.
		//
		FuncVars* parent;

		FuncVars()
		{
			// Defaulted for functions.
			// Used in ScriptCompiler for JUMP location.
			//
			loc = -1;

			parent = nullptr;
		}
	};

	typedef std::map<std::string, FuncVars>  FuncMap;
	typedef std::pair<std::string, FuncVars> FuncPair;

	////////////////////////////////////////////////////////////////////////////////////////
	// Native Function Calls.
	//
	typedef Value(*ScriptFuncPtr0)();
	typedef Value(*ScriptFuncPtr1)(const Value&);
	typedef Value(*ScriptFuncPtr2)(const Value&, const Value&);
	typedef Value(*ScriptFuncPtr3)(const Value&, const Value&, const Value&);
	typedef Value(*ScriptFuncPtr4)(const Value&, const Value&, const Value&, const Value&);

	struct NativeFunction
	{
		std::string name;
		std::string args;

		union
		{
			ScriptFuncPtr0 Func0;
			ScriptFuncPtr1 Func1;
			ScriptFuncPtr2 Func2;
			ScriptFuncPtr3 Func3;
			ScriptFuncPtr4 Func4;
		};

	#define NATIVE_FUNC_HELPER(count)\
		NativeFunction(const std::string& _name, ScriptFuncPtr##count _func, const std::string& _args)\
		{\
			name = _name;\
			Func##count = _func;\
			args = _args;\
		}

		NATIVE_FUNC_HELPER(0)
		NATIVE_FUNC_HELPER(1)
		NATIVE_FUNC_HELPER(2)
		NATIVE_FUNC_HELPER(3)
		NATIVE_FUNC_HELPER(4)

		static int Find(const std::vector<NativeFunction>& funcs, const std::string& name)
		{
			for (int x = 0; x < (int)funcs.size(); ++x)
			{
				if (funcs[x].name.compare(name) == 0)
				{
					return x;
				}
			}

			return -1;
		}

		static int Find(const std::vector<NativeFunction>& funcs, const std::string& name, int count)
		{
			for (int x = 0; x < (int)funcs.size(); ++x)
			{
				if (funcs[x].name.compare(name) == 0)
				{
					if ((int)funcs[x].args.size() == count)
					{
						return x;
					}
				}
			}

			return -1;
		}

		static int Find(const std::vector<NativeFunction>& funcs, const std::string& name, const std::string& args)
		{
			for (int x = 0; x < (int)funcs.size(); ++x)
			{
				if (funcs[x].name.compare(name) == 0)
				{
					if (funcs[x].args.compare(args) == 0)
					{
						return x;
					}
				}
			}

			return -1;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// Analyze tokens for the ScriptParser.
	//
	class ScriptLexer
	{
		unsigned char c;
		char* data;
		char* copy;

	public:

		int linenumber;
		int token;
		Fixed16 numberData;
		std::string stringData;

		//////////////////////////////////////

		ScriptLexer();
		ScriptLexer(const char* filename);

		void Startup();

		int  Load(const char* filename);

		void Shutdown();

		int  Next();
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// Analyze grammar.
	//
	class ScriptParser
	{
		ScriptLexer lex;
		bool didError;

	public:

		std::vector<NativeFunction> nativeFunc;

		FuncVars globals;

		FuncMap funcs;
		FuncVars* currVars;

		std::vector<ScriptNode*> nodes;

		//////////////////////////////////////

		ScriptParser();
		ScriptParser(const char* filename);

		void Startup();

		int Load(const char* filename);

		void Shutdown();

		void Output(std::fstream& file);

		int GetVarOffset(std::string name, FuncVars* vars);

		// Determine if a variable exists within the scope.
		// Returns location in vector; -1 = not found
		//
		int FindVar(std::string name, FuncVars* vars);

	private:

		void Expect(int token);

		ScriptNode* ParseStatement();

		// Parse an expression.
		//
		ScriptNode* ParsePlusExp();
		ScriptNode* ParseMulExp();
		ScriptNode* ParseFactorExp();

		// Parse a variable.
		//
		ScriptNode* ParseVariable(std::string name);
		ScriptNode* ParseVector();

		// Parse a function.
		//
		ScriptNode* ParseFuncParams_INTERNAL();
		ScriptNode* ParseFuncParams();
		ScriptNode* ParseFuncValues();

		// Set a node.
		//
		void SetNode(ScriptNode& node, ScriptNode* node_ptr);
		void SetNode(ScriptNode* node, std::vector<ScriptNode*>& node_vec);

		// Create a variable container.
		//
		void CreateFuncVars(std::string name);
		void CreateFuncVars();
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// ScriptCompiler.
	//
	class ScriptCompiler
	{
		// Counts the number of functions that have been accessed
		// so the Functions can be grabbed from the parser correctly.
		//
		int funcCount;

		// Remember what the last variable that was pushed.
		// Useful for assigning variables by type.
		//
		int lastVar;

		// Storage for function calls that do not have a line number set.
		//
		typedef std::map<std::string, std::vector<int>> CallMap;
		typedef std::pair<std::string, std::vector<int>> CallPair;

		CallMap funcCall;

		std::vector<int> popCount;

	public:

		ScriptParser parser;

		std::vector<UCHAR> data;

		ScriptCompiler();
		ScriptCompiler(const char* filename);

		void Startup();

		int  Load(const char* filename);

		void Shutdown();

		void Output(std::fstream& file);

	private:

		void Traverse(ScriptNode* node);

		// Push back a value to the code byte.
		//
		void EmitC(UCHAR c);
		void EmitC(UCHAR c0, UCHAR c1);
		void EmitCAt(UCHAR c, int index);
		void EmitS(char* str);

		// float/int
		//
		template <typename Real>
		void EmitN(Real n)
		{
			UCHAR num[4];
			NUMtoBYTES(num, n);

			for (int x = 0; x < 4; ++x)
			{
				EmitC(num[x]);
			}
		}

		template <typename Real>
		void EmitN(UCHAR c, Real n)
		{
			EmitC(c);
			EmitN(n);
		}

		template <typename Real>
		void EmitNAt(Real n, int index)
		{
			UCHAR num[4];
			NUMtoBYTES(num, n);

			for (int x = 0; x < 4; ++x)
			{
				data[index + x] = num[x];
			}
		}

		// Count the number of variables in the function,
		// and in the function call to determine if it
		// is being called with the correct number of
		// parameters.
		//
		int CountFuncVars(ScriptNode* node);

		void OutputLineNum(std::fstream& file, int linenum);
		void OutputNumber(std::fstream& file, int& index, int count = 1, bool isFloat = false);
		void OutputString(std::fstream& file, int& index);
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// Script Execution.
	//
	class Script
	{
		ScriptCompiler comp;

		// Stack values.
		//
		std::vector<Value> global;
		std::vector<Value> stack;

		// Vector Access.
		//
		std::vector<int> vecA;

		int IP;	// pointer in byte code
		int FP; // pointer in stack

		std::vector<int> funcPop; // ensures the stack gets popped correctly

		// Used specifically for sleep.
		//
		float m_waitTime;

		UINT m_numGlobals;

	public:

		Script();
		Script(const char* filename);
		~Script();

		void Startup();

		int  Load(const char* filename);

		void Shutdown();

		void Output(std::fstream& file);
		void Output(char* filename);

		void  SetWaitTime(float _waitTime);
		float WaitTime();

		UINT NumGlobals();

		void CopyStack(std::vector<Value>* globalCopy, std::vector<Value>* stackCopy);
		void SetStack(std::vector<Value>* globalCopy, std::vector<Value>* stackCopy);

		// Returns next instruction location to be executed; SCRIPT_END = end of script
		//
		int Execute(float deltaTime = 0, int _IP = SCRIPT_CONTINUE, std::vector<Value>* globalCopy = nullptr, std::vector<Value>* stackCopy = nullptr);

		void RegisterVar(const std::string& name, const Value& _value);

	#define REGISTER_FUNC_HELPER(count)\
		void RegisterFunc(const std::string& name, ScriptFuncPtr##count func, const std::string& args);

		REGISTER_FUNC_HELPER(0)
		REGISTER_FUNC_HELPER(1)
		REGISTER_FUNC_HELPER(2)
		REGISTER_FUNC_HELPER(3)
		REGISTER_FUNC_HELPER(4)

	private:

		void PrintValue(const Value& v);
	};


	////////////////////////////////////////////////////////////////////////////////////////

	class ScriptException : public std::exception
	{
		int linenumber;
		std::string title;
		std::string msg;

	public:

		ScriptException(int linenumber, std::string title, std::string msg)
		{
			this->linenumber = linenumber;
			this->title = title;
			this->msg = msg;
		}

		virtual const char* what() const throw()
		{
			REPORT_SCRIPT_ERROR(linenumber, title, msg);
			return "Ending Script...\n";
		}
	};
}
