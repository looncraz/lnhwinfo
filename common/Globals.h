#pragma once

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <deque>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>

#if __cplusplus >= 201402L
#	include <filesystem>
namespace fs = std::filesystem;
#elif __cplusplus > 199711L
#	include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#	error "Unsupported C++ version! Set C++11 or newer!"
#endif


/*
	No while (true) loops!
	while (gIsRunning) instead!
*/
extern bool gIsRunning;


#define WINDOW_WIDTH	180.0
#define MAX_UPDATE_RATE 100 // in milliseconds (10/sec)

using time_ms_t = std::chrono::milliseconds;
using time_point_t = std::chrono::time_point<std::chrono::system_clock>;

#	define uint64 unsigned long long
#	define uint32 unsigned long
#	define int64 long long
#	define int32 long
#	define int16 short
#	define uint8 uint8_t

typedef const std::string	CString;
typedef std::string			SString;
typedef std::deque<SString> SStringList;

struct HWPoint {
	int	x;
	int y;
};

template<typename T>
class SStringMap : public std::map<SString, T>{};

typedef struct rgb_color {
	uint8		red;
	uint8		green;
	uint8		blue;
	uint8		alpha;

	inline float rf() const { return (float)red / 255.0;	}
	inline float gf() const { return (float)green / 255.0;	}
	inline float bf() const { return (float)blue / 255.0;	}
	inline float af() const { return (float)alpha / 255.0;	}

	// some convenient additions
	inline rgb_color&
	set_to(uint8 r, uint8 g, uint8 b, uint8 a = 255)
	{
		red = r;
		green = g;
		blue = b;
		alpha = a;
		return *this;
	}

	inline bool
	operator==(const rgb_color& other) const
	{
		return *(const uint32 *)this == *(const uint32 *)&other;
	}

	inline bool
	operator!=(const rgb_color& other) const
	{
		return *(const uint32 *)this != *(const uint32 *)&other;
	}

	inline rgb_color&
	operator=(const rgb_color& other)
	{
		return set_to(other.red, other.green, other.blue, other.alpha);
	}

} rgb_color;


template <typename T, typename I>
auto ContainerFindItem(T& t, I i)
{
	return std::find(t.begin(), t.end(), i);
}


template <typename T, typename I>
bool ContainerHasItem(T& t, I i)
{
	return 	ContainerFindItem(t, i) != t.end();
}

#include "FSWatcher.h"
#include "Object.h"
#include "Utils.h"
