#pragma once

#include <config.hpp>
#include <vendor/stdio.hpp>

template <typename T> inline T min( T a, T b )
{
	return a < b ? a : b;
}