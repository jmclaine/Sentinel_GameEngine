#pragma once

#include <sstream>

namespace Sentinel
{
	class StringStream
	{
	private:

		std::stringstream stream;

	public:

		operator std::string() const { return stream.str(); }

		template<class Type>
		StringStream& operator << (Type const& var) { stream << var; return *this; }

		std::stringbuf* rdbuf() { return stream.rdbuf(); }
	};

#define STREAM(text)\
	(StringStream() << text)
}
