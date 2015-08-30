// Reads a script from a file, and compiles it into bytecode.
//
#include <string>
#include <list>
#include <iostream>

#include "Util.h"
#include "Script.h"
#include "FileUtil.h"

namespace Sentinel
{
#define TRAVERSE_ARRAY(count, var)\
	for (int x = 0; x < count; ++x)\
	{\
		Traverse(var[x]);\
	}

	// ScriptCompiler.
	//
	ScriptCompiler::ScriptCompiler()
	{
		Startup();
	}

	ScriptCompiler::ScriptCompiler(const char* filename)
	{
		Startup();
		Load(filename);
	}

	// Zero out.
	//
	void ScriptCompiler::Startup()
	{
		funcCount = 0;
		lastVar = TOKEN_NUMBER;
	}

	// Load the script into memory.
	//
	int ScriptCompiler::Load(const char* filename)
	{
		try
		{
			if (parser.Load(filename) == 0)
			{
				Shutdown();
				return 0;
			}

			// Push the number of global variables.
			//
			EmitN((UCHAR)OP_PUSH_ADDR_G, (int)parser.globals.vars.size());

			// Create storage for function calls to save their location in the byte code.
			//
			TRAVERSE_LIST(it, parser.funcs)
			{
				funcCall.insert(CallPair(it->first, std::vector< int >()));
			}

			// Create binary data using post order traversal.
			//
			int count = (int)parser.nodes.size();
			for (int x = 0; x < count; ++x)
			{
				Traverse(parser.nodes[x]);
				parser.currVars = NULL;
			}

			// Set all the function call positions.
			//
			TRAVERSE_LIST(it, funcCall)
			{
				count = it->second.size();

				for (int x = 0; x < count; ++x)
				{
					EmitNAt((int)parser.funcs.find(it->first)->second.loc, it->second[x]);
				}
			}

			EmitC((UCHAR)OP_END);
		}
		catch (ScriptException& e)
		{
			Debug::Log(e.what());
			return 0;
		}

		return 1;
	}

	// Release node memory.
	//
	void ScriptCompiler::Shutdown()
	{
		data.clear();
		parser.Shutdown();
	}

	// Output debug data.
	//
	void ScriptCompiler::Output(std::fstream& file)
	{
		OUTPUT_INFO("Output Parsing...\n\n")
			parser.Output(file);

		OUTPUT_INFO("\n\n");
		OUTPUT_INFO("Output Bytecode...\n\n");

		int count = (int)data.size();
		for (int x = 0; x < count; ++x)
		{
			OutputLineNum(file, x);
			switch ((int)data[x])
			{
			case OP_PUSH_NUM:
				OUTPUT_INFO("PUSH NUM");
				OutputNumber(file, x, 1, true);
				break;

			case OP_PUSH_STR:
				OUTPUT_INFO("PUSH STR");
				OutputString(file, x);
				break;

			case OP_PUSH_VAR:
				OUTPUT_INFO("PUSH VAR");
				OutputNumber(file, x);
				break;

			case OP_PUSH_VAR_G:
				OUTPUT_INFO("PUSH VARG");
				OutputNumber(file, x);
				break;

			case OP_PUSH_VEC:
				OUTPUT_INFO("PUSH_VEC");
				OutputNumber(file, x);
				break;

			case OP_VEC_A:
				OUTPUT_INFO("VEC_A\n");
				break;

			case OP_ADD:
				OUTPUT_INFO("ADD\n");
				break;

			case OP_SUB:
				OUTPUT_INFO("SUB\n");
				break;

			case OP_MUL:
				OUTPUT_INFO("MUL\n");
				break;

			case OP_DIV:
				OUTPUT_INFO("DIV\n");
				break;

			case OP_STORE:
				OUTPUT_INFO("STORE\t");
				OutputNumber(file, x);
				break;

			case OP_STORE_G:
				OUTPUT_INFO("STORE G\t");
				OutputNumber(file, x);
				break;

			case OP_INCREMENT:
				break;

			case OP_DECREMENT:
				break;

			case OP_OR:
				OUTPUT_INFO("OR\n");
				break;

			case OP_AND:
				OUTPUT_INFO("AND\n");
				break;

			case OP_NOT:
				OUTPUT_INFO("NOT\n");
				break;

			case OP_EQ:
				OUTPUT_INFO("EQ\n");
				break;

			case OP_NOT_EQ:
				OUTPUT_INFO("NOT EQ\n");
				break;

			case OP_GT:
				OUTPUT_INFO("GT\n");
				break;

			case OP_GT_EQ:
				OUTPUT_INFO("GT EQ\n");
				break;

			case OP_LT:
				OUTPUT_INFO("LT\n");
				break;

			case OP_LT_EQ:
				OUTPUT_INFO("LT EQ\n");
				break;

			case OP_PUSH_ADDR:
				OUTPUT_INFO("PUSH ADDR");
				OutputNumber(file, x);
				break;

			case OP_PUSH_ADDR_G:
				OUTPUT_INFO("PUSH ADDRG");
				OutputNumber(file, x);
				break;

			case OP_POP:
				OUTPUT_INFO("POP\t\t");
				OutputNumber(file, x);
				break;

			case OP_IFNE:
				OUTPUT_INFO("IFNE\t");
				OutputNumber(file, x);
				break;

			case OP_JUMP:
				OUTPUT_INFO("JUMP\t");
				OutputNumber(file, x);
				break;

			case OP_CALL:
				OUTPUT_INFO("CALL\t");
				OutputNumber(file, x, 2);
				break;

			case OP_RETURN:
				OUTPUT_INFO("RETURN\t");
				OutputNumber(file, x);
				break;

			case OP_PRINT:
				OUTPUT_INFO("PRINT\n");
				break;

			case OP_SLEEP:
				OUTPUT_INFO("SLEEP\n");
				break;

			case OP_PAUSE:
				OUTPUT_INFO("PAUSE\n");
				break;

			case OP_NATIVE_FUNC:
				OUTPUT_INFO("NATIVE\t");
				OutputString(file, x);
				break;

			case OP_END:
				OUTPUT_INFO("END\n");
				break;

			default:
				OUTPUT_INFO("UNKNOWN: %s\n", TokenToString((int)data[x]));
			}
		}
	}

	// Traverse through the tree.
	//
	void ScriptCompiler::Traverse(ScriptNode* node)
	{
		if (node == NULL)
		{
			return;
		}

		// Function CALL without return value assigned.
		//
		if (node->type == TOKEN_VOID)
		{
			Traverse(node->child.child);
			Traverse(node->sibling);
			EmitN((UCHAR)OP_POP, 1);
			return;
		}
		else if (node->type == TOKEN_FUNC_CALL)
		{
			Traverse(node->sibling);
			Traverse(node->child.child);
			return;
		}

		// Check the sibling.
		//
		if (node->type != TOKEN_IF && node->type != TOKEN_WHILE && node->type != TOKEN_FOR)
		{
			// Check if the node is a statement container,
			// and push variables into the map.
			//
			if (node->type == TOKEN_LCURLY)
			{
				char buf[256];
				sprintf_s(buf, "~%d", funcCount);
				parser.currVars = &parser.funcs.find(buf)->second;

				int pop = (int)parser.currVars->vars.size();
				EmitN((UCHAR)OP_PUSH_ADDR, pop);

				popCount.push_back(pop);

				++funcCount;

				TRAVERSE_ARRAY((int)*node->child.numberData, &node->sibling);
			}
			// Vector List.
			//
			else if (node->type == TOKEN_VECTOR_LIST)
			{
				TRAVERSE_ARRAY((int)*node->child.numberData, &node->sibling);
			}
			// Functions.
			//
			else if (node->type == TOKEN_FUNCTION)
			{
				// Function call.
				//
				if (node->sibling[1].type != TOKEN_LCURLY)
				{
					FuncVars* refVars = parser.currVars;

					// Find the function.
					//
					FuncMap::iterator tempMap = parser.funcs.find(node->child.stringData);
					if (tempMap == parser.funcs.end())
					{
						THROW_SCRIPT_EXCEPTION(0, "ERROR", "Function '%s' used without declaration.", node->child.stringData);
					}

					// Push values on the stack from the function.
					//
					Traverse(&node->sibling[0]);

					// Set current variables to the current function.
					//
					parser.currVars = &tempMap->second;

					// Count the number of variables used in this function to determine if this is a valid call.
					//
					//countFuncVars( node );

					// Push the function location.
					//
					int loc = parser.currVars->loc;

					EmitN((UCHAR)OP_CALL, (int)loc);
					EmitN((int)parser.currVars->vars.size());

					if (loc == -1)
					{
						// Push the line and function for forward declarations.
						//
						funcCall.find(node->child.stringData)->second.push_back((int)data.size() - 4);
					}

					parser.currVars = refVars;
				}
				// Function declaration.
				//
				else
				{
					// Find the function.
					//
					parser.currVars = &parser.funcs.find(node->child.stringData)->second;

					// Add to the function counter.
					//
					++funcCount;

					// Jump over this function because it is not called here.
					//
					EmitN((UCHAR)OP_JUMP, 0);
					int loc = data.size() - 4;

					// Set its current location.
					//
					parser.currVars->loc = (int)data.size();

					// Traverse the statement.
					//
					Traverse(&node->sibling[1]);

					// The last call should be RETURN instead of POP.
					//
					int index = data.size() - 5;

					EmitCAt((UCHAR)OP_PUSH_NUM, index);

					// Save the POP value.
					//
					++index;
					UCHAR num[4];
					for (int y = 0; y < 4; ++y)
					{
						num[y] = data[index + y];
					}
					int value;
					BYTEStoNUM(num, value);

					// Output a default 0.0f value.
					//
					EmitNAt(0, index);

					// Ensure the POP for RETURN is set.
					//
					EmitN((UCHAR)OP_RETURN, value);

					// Set ending position of the function.
					//
					EmitNAt((int)data.size() - 1, loc);

					parser.currVars = parser.currVars->parent;
				}
				return;
			}
			// Native Functions.
			//
			else if (node->type == TOKEN_NATIVE_FUNC)
			{
				int index = (int)*node->child.numberData;

				// Push values on the stack from the function.
				//
				Traverse(node->sibling);

				// Count the number of variables used in this function to determine if this is a valid call.
				//
				int count = CountFuncVars(node);
				index = NativeFunction::Find(parser.nativeFunc, parser.nativeFunc[index].name, count);

				if (index == -1)
				{
					THROW_SCRIPT_EXCEPTION(0, "ERROR", "Native Function '%s' used with %d arguments.", \
						parser.nativeFunc[(int)*node->child.numberData].name.c_str(), count);
				}

				*node->child.numberData = index;
				EmitC((UCHAR)OP_NATIVE_FUNC);
				EmitC((UCHAR)index);
				return;
			}
			// Store a value.
			//
			else if (node->type == TOKEN_ASSIGN || node->type == TOKEN_ADD_ASSIGN || node->type == TOKEN_SUB_ASSIGN ||
				node->type == TOKEN_MUL_ASSIGN || node->type == TOKEN_DIV_ASSIGN)
			{
				UCHAR pushType = (UCHAR)OP_PUSH_VAR;
				UCHAR storeType = (UCHAR)OP_STORE;
				int index = 0;

				if (parser.currVars != NULL)
				{
					index = parser.GetVarOffset(node->child.child->child.stringData, parser.currVars);
				}

				if (index == 0)
				{
					index = (parser.GetVarOffset(node->child.child->child.stringData, &parser.globals) + 2) * -1;
					pushType = (UCHAR)OP_PUSH_VAR_G;
					storeType = (UCHAR)OP_STORE_G;
				}

				if (node->type == TOKEN_ADD_ASSIGN || node->type == TOKEN_SUB_ASSIGN ||
					node->type == TOKEN_MUL_ASSIGN || node->type == TOKEN_DIV_ASSIGN)
				{
					EmitN(pushType, index);
				}

				Traverse(node->sibling);
				Traverse(node->child.child->sibling);

				// Store a value with an added operation.
				//
				switch (node->type)
				{
				case TOKEN_ADD_ASSIGN:
					EmitC((UCHAR)OP_ADD);
					break;

				case TOKEN_SUB_ASSIGN:
					EmitC((UCHAR)OP_SUB);
					break;

				case TOKEN_MUL_ASSIGN:
					EmitC((UCHAR)OP_MUL);
					break;

				case TOKEN_DIV_ASSIGN:
					EmitC((UCHAR)OP_DIV);
					break;
				}

				EmitN(storeType, index);
				return;
			}
			else if (node->sibling != NULL)
			{
				Traverse(node->sibling);
			}
		}

		// Traverse the child only if it is a node.
		//
		if (node->child.child != NULL && node->type != TOKEN_STRING && node->type != TOKEN_VAR && node->type != TOKEN_NUMBER &&
			node->type != TOKEN_LCURLY && node->type != TOKEN_VECTOR_LIST && node->type != TOKEN_RETURN &&
			node->type != TOKEN_IF     && node->type != TOKEN_FOR         && node->type != TOKEN_WHILE  &&
			node->type != TOKEN_PRINT  && node->type != TOKEN_SLEEP       && node->type != TOKEN_PAUSE)
		{
			Traverse(node->child.child);
		}

		// Determine what to do with this node.
		//
		switch (node->type)
		{
		case TOKEN_NUMBER:
			EmitN((UCHAR)OP_PUSH_NUM, node->child.numberData->value);
			lastVar = TOKEN_NUMBER;
			break;

		case TOKEN_STRING:
			EmitC((UCHAR)OP_PUSH_STR);
			EmitS(node->child.stringData);
			// NULL terminated
			lastVar = TOKEN_STRING;
			break;

		case TOKEN_VECTOR_LIST:
			EmitN((UCHAR)OP_PUSH_VEC, node->child.numberData->value);
			lastVar = TOKEN_VECTOR_LIST;
			break;

		case TOKEN_VECTOR_ACCESS:
			EmitC((UCHAR)OP_VEC_A);
			break;

		case TOKEN_ADD:
			EmitC((UCHAR)OP_ADD);
			break;

		case TOKEN_SUB:
			EmitC((UCHAR)OP_SUB);
			break;

		case TOKEN_MUL:
			EmitC((UCHAR)OP_MUL);
			break;

		case TOKEN_DIV:
			EmitC((UCHAR)OP_DIV);
			break;

		case TOKEN_OR:
			EmitC((UCHAR)OP_OR);
			break;

		case TOKEN_AND:
			EmitC((UCHAR)OP_AND);
			break;

		case TOKEN_NOT:
			EmitC((UCHAR)OP_NOT);
			break;

		case TOKEN_EQ:
			EmitC((UCHAR)OP_EQ);
			break;

		case TOKEN_NOT_EQ:
			EmitC((UCHAR)OP_NOT_EQ);
			break;

		case TOKEN_GT:
			EmitC((UCHAR)OP_GT);
			break;

		case TOKEN_GT_EQ:
			EmitC((UCHAR)OP_GT_EQ);
			break;

		case TOKEN_LT:
			EmitC((UCHAR)OP_LT);
			break;

		case TOKEN_LT_EQ:
			EmitC((UCHAR)OP_LT_EQ);
			break;

		case TOKEN_VAR:
		{
			UCHAR pushType = (UCHAR)OP_PUSH_VAR;
			int index = 0;

			if (parser.currVars != NULL)
			{
				index = parser.GetVarOffset(node->child.stringData, parser.currVars);
			}
			if (index == 0)
			{
				index = (parser.GetVarOffset(node->child.stringData, &parser.globals) + 2) * -1;
				pushType = (UCHAR)OP_PUSH_VAR_G;
			}
			EmitN(pushType, index);
		}
		break;

		case TOKEN_IF:
		{
			Traverse(node->sibling);
			EmitN((UCHAR)OP_IFNE, 0);
			int loc0 = data.size() - 4;
			Traverse(&node->child.child[0]);

			if (node->child.child[1].type != TOKEN_NONE)
			{
				EmitN((UCHAR)OP_JUMP, 0);
				int loc1 = data.size() - 4;
				EmitNAt((int)data.size() - 1, loc0);
				Traverse(&node->child.child[1]);
				EmitNAt((int)data.size() - 1, loc1);
			}
			else
			{
				EmitNAt((int)data.size() - 1, loc0);
			}
		}
		break;

		case TOKEN_WHILE:
		{
			int loc0 = data.size();
			Traverse(node->sibling);
			EmitN((UCHAR)OP_IFNE, 0);
			int loc1 = data.size() - 4;
			Traverse(node->child.child);
			EmitN((UCHAR)OP_JUMP, loc0 - 1);
			EmitNAt((int)data.size() - 1, loc1);
		}
		break;

		case TOKEN_FOR:
		{
			Traverse(&node->sibling[0]);
			int loc0 = data.size();
			Traverse(&node->sibling[1]);
			EmitN((UCHAR)OP_IFNE, 0);
			int loc1 = data.size() - 4;
			Traverse(node->child.child);
			Traverse(&node->sibling[2]);
			EmitN((UCHAR)OP_JUMP, loc0 - 1);
			EmitNAt((int)data.size() - 1, loc1);
		}
		break;

		case TOKEN_LCURLY:
			EmitN((UCHAR)OP_POP, popCount.back());
			popCount.pop_back();
			break;

		case TOKEN_RETURN:
		{
			int count = 0;
			for (int x = 0; x < (int)popCount.size(); ++x)
			{
				count += popCount[x];
			}
			EmitN((UCHAR)OP_RETURN, count);
		}
		break;

		case TOKEN_PRINT:
			EmitC((UCHAR)OP_PRINT);
			break;

		case TOKEN_SLEEP:
			EmitC((UCHAR)OP_SLEEP);
			break;

		case TOKEN_PAUSE:
			EmitC((UCHAR)OP_PAUSE);
			break;
		}
	}

	// Push back a value to the code byte.
	//
	void ScriptCompiler::EmitC(UCHAR c)
	{
		data.push_back(c);
	}

	void ScriptCompiler::EmitC(UCHAR c0, UCHAR c1)
	{
		EmitC(c0);
		EmitC(c1);
	}

	void ScriptCompiler::EmitCAt(UCHAR c, int index)
	{
		data[index] = c;
	}

	void ScriptCompiler::EmitS(char* str)
	{
		int len = strlen(str);
		for (int x = 0; x < len; ++x)
		{
			EmitC((UCHAR)str[x]);
		}
		EmitC(0);
	}

	// Count the number of variables in the function.
	//
	int ScriptCompiler::CountFuncVars(ScriptNode* node)
	{
		int varCount = 0;
		ScriptNode* n = node->sibling;

		while (n != NULL)
		{
			++varCount;
			n = n->sibling;
		}

		return varCount;
	}

	void ScriptCompiler::OutputLineNum(std::fstream& file, int linenum)
	{
		OUTPUT_INFO("%d:\t", linenum);

		if (linenum < 100)
		{
			OUTPUT_INFO("\t");
		}
	}

	void ScriptCompiler::OutputNumber(std::fstream& file, int& index, int count, bool isFloat)
	{
		UCHAR num[4];
		for (int x = 0; x < count; ++x)
		{
			OUTPUT_INFO("\t");
			for (int y = 0; y < 4; ++y)
			{
				++index;
				num[y] = data[index];
			}

			if (isFloat)
			{
				float value;
				BYTEStoNUM(num, value);
				OUTPUT_INFO("%f", value);
			}
			else
			{
				int value;
				BYTEStoNUM(num, value);
				OUTPUT_INFO("%d", value);
			}
		}
		OUTPUT_INFO("\n");
	}

	void ScriptCompiler::OutputString(std::fstream& file, int& index)
	{
		OUTPUT_INFO("\t");
		while (data[++index] != 0)
		{
			OUTPUT_INFO("%c", data[index]);
		}
		OUTPUT_INFO("\n");
	}
}