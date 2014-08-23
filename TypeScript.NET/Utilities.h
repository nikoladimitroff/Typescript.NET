#pragma once
#include <ostream>
#include <set>
#if defined(_WIN32)
	#define export __declspec(dllexport)
	#define import __declspec(dllimport)
#else
	#define export __attribute__ ((visibility ("default")))
	#define import
#endif

#if defined(CLIENT_CODE)
	#define dllspec import
#else
	#define dllspec export
#endif

template<typename T>
std::ostream& JoinCollection(const T& collection, std::ostream& stream, const std::string& separator = ", ")
{
	bool first = true;
	for (auto s : collection)
	{
		if (!first)
		{
			stream << separator;
		}
		stream << s;
		first = false;
	}
	return stream;
}

template <class Key, class Compare, class Alloc, class Func>
void erase_if(std::set<Key, Compare, Alloc>& set, Func f)
{
	for (typename std::set<Key, Compare, Alloc>::iterator it = set.begin(); it != set.end();) {
		if (f(*it)) {
			it = set.erase(it);
		}
		else {
			++it;
		}
	}
}