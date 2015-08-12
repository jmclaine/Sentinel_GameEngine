#include "Tokenizer.h"

namespace Sentinel
{
	const std::string Tokenizer::DELIMITERS(" \t\n\r");

	Tokenizer::Tokenizer(const std::string& s) :
		mString(s), 
		mOffset(0), 
		mDelimiters(DELIMITERS)
	{}

	Tokenizer::Tokenizer(const std::string& s, const std::string& delimiters) :
		mString(s), 
		mOffset(0), 
		mDelimiters(delimiters)
	{}

	bool Tokenizer::NextToken() 
	{
		return NextToken(mDelimiters);
	}

	bool Tokenizer::NextToken(const std::string& delimiters) 
	{
		size_t i = mString.find_first_not_of(delimiters, mOffset);
		if (std::string::npos == i) 
		{
			mOffset = mString.length();
			return false;
		}

		size_t j = mString.find_first_of(delimiters, i);
		if (std::string::npos == j) 
		{
			mToken = mString.substr(i);
			mOffset = mString.length();
			return true;
		}

		mToken = mString.substr(i, j - i);
		mOffset = j;
		return true;
	}

	const std::string& Tokenizer::GetToken() const
	{
		return mToken;
	}

	void Tokenizer::Reset()
	{
		mOffset = 0;
	}
}
