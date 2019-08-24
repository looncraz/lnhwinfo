#pragma once

#include <chrono>
#include <deque>
#include <functional>
#include <map>
#include <string>


#define WINDOW_WIDTH	180.0

using time_ms_t = std::chrono::milliseconds;
using time_point_t = std::chrono::time_point<std::chrono::system_clock>;

#	define uint64 unsigned long long
#	define uint32 unsigned long
#	define int32 long
#	define int16 short
#	define uint8 uint8_t

typedef const std::string 	CString;
typedef std::string 		SString;
typedef std::deque<SString> SStringList;

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

#include "Utils.h"
