#pragma once

#include <types.hpp>
#include <debug.hpp>

#include <build/memory.hpp>
#include <build/color.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Texture2DBuffer
{
	Texture2DBuffer() : data( nullptr ), width( 0 ), height( 0 ) { }

	bool save( const char *path );
	bool load( const char *path );

	void init( const u16 width, const u16 height );
	void free();
	void copy( const Texture2DBuffer &other );
	void move( Texture2DBuffer &&other );

	void splice( Texture2DBuffer &source, const u16 srcX1, const u16 srcY1, const u16 srcX2, const u16 srcY2, const u16 dstX, const u16 dstY );
	inline void splice( Texture2DBuffer &source, const u16 dstX, const u16 dstY ) { splice( source, 0, 0, source.width, source.height, dstX, dstY ); }

	void clear( const rgba color );

	explicit operator bool() const { return data != nullptr; }

	inline rgba &at( const u32 index )
	{
		Assert( index < static_cast<u32>( width ) * static_cast<u32>( height ) );
		return data[index];
	}

	inline rgba &at( const u16 x, const u16 y )
	{
		Assert( x < width && y < height );
		return data[ y * width + x ];
	}

	inline void set( const u32 index, const rgba value )
	{
		Assert( index < static_cast<u32>( width ) * static_cast<u32>( height ) );
		data[index] = value;
	}

	inline void set( const u16 x, const u16 y, const rgba value )
	{
		Assert( x < width && y < height );
		data[ y * width + x ] = value;
	}

	inline rgba &operator[]( const u32 index ) { return at( index ); }

public:
	rgba *data = nullptr;
	u16 width;
	u16 height;
};