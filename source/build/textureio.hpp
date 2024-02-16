#pragma once

#include <types.hpp>
#include <debug.hpp>

#include <build/memory.hpp>
#include <build/color.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Texture2DBuffer
{
	Texture2DBuffer() : data( nullptr ), width( 0 ), height( 0 ) { }
	Texture2DBuffer( const u16 width, const u16 height ) { init( width, height ); }
	Texture2DBuffer( const char *path ) { load( path ); }
	Texture2DBuffer( const Texture2DBuffer &other ) { copy( other ); }
	Texture2DBuffer( Texture2DBuffer &&other ) { move( static_cast<Texture2DBuffer &&>( other ) ); }
	~Texture2DBuffer() { free(); }

	Texture2DBuffer &operator=( const Texture2DBuffer &other ) { copy( other ); return *this; }
	Texture2DBuffer &operator=( Texture2DBuffer &&other ) { move( static_cast<Texture2DBuffer &&>( other ) ); return *this; }

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

// Data
	rgba *data = nullptr;
	u16 width;
	u16 height;
};