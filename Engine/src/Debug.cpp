#include "Debug.h"
#include <sstream>

#ifndef WIN32
#define OutputDebugStringA(str)\
	printf("%s\n", str);

#define MessageBoxA(ptr, msg, title, flags)\
	printf("%s: %s\n", title, msg);

#define strcat_s strcat
#define sprintf_s sprintf
#else
#include <windows.h>
#endif

namespace Sentinel
{
#define GET_LOG_VAR(append, stream, var)\
	std::stringstream _##stream;\
	_##stream << append << stream.rdbuf() << std::endl;\
	const std::string& var = _##stream.str();
	
#define GET_LOG(stream)\
	GET_LOG_VAR("", stream, text)\
	OutputDebugStringA(text.c_str());

#define GET_WARNING(stream)\
	GET_LOG_VAR("WARNING: ", stream, text)\
	OutputDebugStringA(text.c_str());

#define GET_ERROR(stream)\
	GET_LOG_VAR("ERROR: ", stream, text)\
	OutputDebugStringA(text.c_str());

	void Debug::Log(const std::string& text)
	{
		Log(STREAM(text));
	}

	void Debug::Log(StringStream& stream)
	{
		GET_LOG(stream);
	}

	void Debug::LogWarning(const std::string& text)
	{
		LogWarning(STREAM(text));
	}

	void Debug::LogWarning(StringStream& stream)
	{
		GET_WARNING(stream);
	}

	void Debug::ShowWarning(const std::string& msg, const std::string& title)
	{
		ShowWarning(STREAM(msg), STREAM(title));
	}

	void Debug::ShowWarning(StringStream& msg, StringStream& title)
	{
		GET_LOG_VAR("", msg, msgStr);
		GET_LOG_VAR("", title, titleStr);

		MessageBoxA(NULL, msgStr.c_str(), titleStr.c_str(), MB_OK | MB_ICONWARNING);
	}

	void Debug::LogError(const std::string& text)
	{
		LogError(STREAM(text));
	}

	void Debug::LogError(StringStream& stream)
	{
		GET_ERROR(stream);
	}

	void Debug::ShowError(const std::string& msg, const std::string& title)
	{
		ShowError(STREAM(msg), STREAM(title));
	}

	void Debug::ShowError(StringStream& msg, StringStream& title)
	{
		GET_LOG_VAR("", msg, msgStr);
		GET_LOG_VAR("", title, titleStr);

		MessageBoxA(NULL, msgStr.c_str(), titleStr.c_str(), MB_OK | MB_ICONERROR);
	}
}
