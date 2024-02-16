#pragma once

#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct rgba
{
	rgba( u8 r, u8 g, u8 b, u8 a ) : r(r), g(g), b(b), a(a) { }
	inline bool operator==( const rgba &other ) const { return r == other.r && g == other.g && b == other.b && a == other.a; }

	u8 r, g, b, a;
};

static_assert( sizeof( rgba ) == 4, "RGBA type not 4 bytes" );