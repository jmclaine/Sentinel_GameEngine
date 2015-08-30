#pragma once

#include "Sentinel.h"

namespace Sentinel
{
	// Based on code from vzczc @
	// http://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
	//
	class SENTINEL_DLL Tokenizer 
	{
	public:

		static const std::string DELIMITERS;

		Tokenizer(const std::string& str);
		Tokenizer(const std::string& str, const std::string& delimiters);

		bool NextToken();
		bool NextToken(const std::string& delimiters);

		const std::string& GetToken() const;

		void Reset();

	protected:

		size_t mOffset;
		const std::string mString;
		std::string mToken;
		std::string mDelimiters;
	};
}
