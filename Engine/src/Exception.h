#pragma once

#include <exception>
#include <string>

namespace Sentinel
{
	class AppException : public std::exception
	{
		std::string mMessage;

	public:

		AppException(const std::string& msg)
		{
			mMessage = msg;
		}

		const char* what() const throw()
		{
			return mMessage.c_str();
		}
	};
}
