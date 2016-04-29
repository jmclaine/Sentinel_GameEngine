// Node used in the parser.
//
#include "Script.h"
#include "Memory.h"
#include "Util.h"

namespace Sentinel
{
	ScriptNode::ScriptNode()
	{
		type = TOKEN_NONE;
		sibling = nullptr;
		child.child = nullptr;
	}

	ScriptNode::ScriptNode(int type)
	{
		this->type = type;
		sibling = nullptr;
		child.child = nullptr;
	}

	ScriptNode::ScriptNode(int type, ScriptNode* sibling)
	{
		this->type = type;
		this->sibling = sibling;
		child.child = nullptr;
	}

	ScriptNode::ScriptNode(int type, ScriptNode* sibling, ChildData child)
	{
		this->type = type;
		this->sibling = sibling;
		this->child = child;
	}

	void ScriptNode::Shutdown()
	{
		if (type == TOKEN_VECTOR_LIST || type == TOKEN_LCURLY)
		{
			SHUTDOWN_ARRAY((int)(*child.numberData), sibling);
		}
		else if (type == TOKEN_FOR)
		{
			SHUTDOWN_ARRAY(3, sibling);
		}
		else if (type == TOKEN_FUNCTION)
		{
			SHUTDOWN_ARRAY(2, sibling);
		}
		else if (sibling != nullptr)
		{
			sibling->Shutdown();
			delete sibling;
		}
		sibling = nullptr;

		if (type == TOKEN_VAR || type == TOKEN_STRING || type == TOKEN_FUNCTION || type == TOKEN_NATIVE_FUNC)
		{
			delete[] child.stringData;
		}
		else if (type == TOKEN_NUMBER || type == TOKEN_BOOL || type == TOKEN_VECTOR_LIST || type == TOKEN_LCURLY)
		{
			delete child.numberData;
		}
		else if (child.child != nullptr && type != TOKEN_LCURLY)
		{
			if (type == TOKEN_IF)
			{
				SHUTDOWN_ARRAY(2, child.child);
			}
			else
			{
				child.child->Shutdown();
				delete child.child;
			}
		}
		child.child = nullptr;
	}

	void ScriptNode::Output(std::fstream& file, int width)
	{
		for (int x = 0; x < width; ++x)
		{
			OUTPUT_INFO("--->\t");
		}

		OUTPUT_INFO("%s\n", TokenToString(type));

		if (sibling != nullptr)
		{
			if (type == TOKEN_LCURLY || type == TOKEN_VECTOR_LIST)
			{
				OUTPUT_ARRAY((int)*child.numberData, sibling);
			}
			else if (type == TOKEN_FOR)
			{
				OUTPUT_ARRAY(3, sibling);
			}
			else if (type == TOKEN_FUNCTION)
			{
				OUTPUT_ARRAY(2, sibling);
			}
			else
			{
				sibling->Output(file, width + 1);
			}
		}

		// Output the child only if it is a node.
		//
		if (child.child != nullptr && type != TOKEN_STRING && type != TOKEN_VAR && type != TOKEN_NUMBER && type != TOKEN_BOOL &&
			type != TOKEN_LCURLY && type != TOKEN_FUNCTION && type != TOKEN_VECTOR_LIST && type != TOKEN_NATIVE_FUNC)
		{
			if (type == TOKEN_IF)
			{
				OUTPUT_ARRAY(2, child.child);
			}
			else
			{
				child.child->Output(file, width + 1);
			}
		}
	}

	// Child data sets.
	//
	union ScriptNode::ChildData SetData(ScriptNode* child)
	{
		union ScriptNode::ChildData data;
		data.child = child;
		return data;
	}

	union ScriptNode::ChildData SetData(Fixed16* child)
	{
		union ScriptNode::ChildData data;
		data.numberData = child;
		return data;
	}

	union ScriptNode::ChildData SetData(const char* child)
	{
		union ScriptNode::ChildData data;
		size_t len = strlen(child) + 1;

		data.stringData = new char[len];
		strcpy_s(data.stringData, len, child);

		return data;
	}
};