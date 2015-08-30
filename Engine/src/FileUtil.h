#pragma once

#include <algorithm>
#include "Sentinel.h"
#include "Types.h"

namespace Sentinel
{
	SENTINEL_DLL void SetDirectory(const char* dest);
	SENTINEL_DLL std::string GetDirectory();

	template <typename Real>
	inline void CopyArray(Real& dest, const Real& src, UINT size)
	{
		for (UINT x = 0; x < size; ++x)
			dest[x] = src[x];
	}

	template <class ForwardIterator, class Type>
	ForwardIterator binary_find(ForwardIterator begin, ForwardIterator end, Type val)
	{
		ForwardIterator i = std::lower_bound(begin, end, val);

		return (i != end && *i == val) ? i : end;
	}

	template <typename Real>
	void NUMtoBYTES(UCHAR* data, Real value)
	{
		unsigned result = *(reinterpret_cast<unsigned*>(&value));

		for (unsigned i = 0; i < 4; ++i)
		{
			data[i] = (result & 0xFF);
			result >>= 8;
		}
	}

	template <typename Real>
	inline void BYTEStoNUM(UCHAR* data, Real& type)
	{
		unsigned result = 0;

		for (unsigned i = 0; i < 4; ++i)
		{
			result = (result << 8);
			result = (result | data[3 - i]);
		}

		type = *(reinterpret_cast<Real*>(&result));
	}
}
