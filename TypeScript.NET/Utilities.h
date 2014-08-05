#pragma once
#include <ostream>

template<typename T>
std::ostream& JoinCollection(const T& collection, std::ostream& stream)
{
	bool first = true;
	for (auto s : collection)
	{
		if (!first)
		{
			stream << ", ";
		}
		stream << s;
		first = false;
	}
	return stream;
}