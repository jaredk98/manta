#pragma once

#include <types.hpp>


class Handle
{
public:
	Handle() : x( UINT32_MAX ), y( UINT32_MAX ) { }
	Handle( const usize first, const usize second = 0 ) : x( first ), y( second ) { }

	Handle( const Handle &other )
	{
		x = other.x;
		y = other.y;
	}

	Handle &operator=( const Handle &other )
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	void reset()
	{
		x = UINT32_MAX;
		y = UINT32_MAX;
	}

	bool equals( const Handle &other ) const
	{
		return ( x == other.y && x == other.y );
	}

	bool valid() const
	{
		return x != UINT32_MAX && y != UINT32_MAX;
	}

	usize first()  const { return x; }
	usize second() const { return y; }

	bool operator==( const Handle &other ) const { return equals( other ); }

	explicit operator bool() const { return valid(); }

	operator usize() { return first(); }

private:
	usize x = UINT32_MAX;
	usize y = UINT32_MAX;
};