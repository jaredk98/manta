#pragma once

#include <types.hpp>
#include <debug.hpp>

#include <build/memory.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Buffer
{
public:
	//Buffer() = default;
	Buffer( usize reserve = 1, const bool grow = true ) { init( reserve, grow ); }
	Buffer( const char *path, const bool grow = true ) { load( path, grow ); }
	Buffer( const Buffer &other ) { copy( other ); }
	Buffer( Buffer &&other ) { move( static_cast<Buffer &&>( other ) ); }
	~Buffer() { free(); }

	Buffer &operator=( const Buffer &other ) { copy( other ); return *this; }
	Buffer &operator=( Buffer &&other ) { move( static_cast<Buffer &&>( other ) ); return *this; }

	void init( usize reserve, const bool grow );
	void free();
	void grow();
	void copy( const Buffer &other );
	void move( Buffer &&other );

	bool save( const char *path );
	bool load( const char *path, const bool grow );

	void write( void *data, const usize size );

	template <typename T>
	void write( const T element )
	{
		while( !fixed )
		{
			if( current + sizeof( T ) < capacity ) { break; }
			grow();
		}

		ErrorIf( current + sizeof( T ) >= capacity, "Failed writing to fixed buffer that is at capacity" );
		memory_copy( data + current, &element, sizeof( T ) );
		current += sizeof( T );
		seek_end();
	}

	template <typename T>
	void poke( const usize tell, const T &element )
	{
		while( !fixed )
		{
			if( tell + sizeof( T ) < capacity ) { break; }
			grow();
		}

		ErrorIf( tell + sizeof( T ) >= capacity, "Failed poking into fixed buffer that is at capacity" );
		memory_copy( data + tell, &element, sizeof( T ) );
	}

	template <typename T>
	T read()
	{
		T element;
		if( this->tell + sizeof( T ) > current ) { return { }; }
		memory_copy( &element, data + this->tell, sizeof( T ) );
		this->tell += sizeof( T );
		return element;
	}

	template <typename T>
	T peek( const usize tell )
	{
		T element;
		if( tell + sizeof( T ) >= current ) { return element; }
		memory_copy( &element, data + tell, sizeof( T ) );
		return element;
	}

	inline void seek_start() { tell = 0; }
	inline void seek_end() { tell = current; }
	inline void seek_to( const usize tell ) { Assert( tell < this->current ); this->tell = tell; }
	inline usize size() { return current; }

public:
	byte *data = nullptr;
	usize tell = 0;

private:
	usize capacity = 0;
	usize current = 0;
	bool fixed = false;
};