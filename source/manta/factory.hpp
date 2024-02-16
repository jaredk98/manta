#pragma once

#include <types.hpp>
#include <debug.hpp>

#include <manta/memory.hpp>
#include <manta/handle.hpp>

#include <vendor/stdio.hpp>


template <typename T, bool Scoped = false> class Factory;


template <typename T>
class Factory<T, false>
{
public:
	Factory() : data( nullptr ), capacity( 0 ), current( 0 ) { }
	~Factory() { }

	Factory( const Factory<T> &other ) { copy( other ); }
	Factory( Factory<T> &&other ) { move( static_cast<Factory<T> &&>( other ) ); }

	Factory<T> &operator=( const Factory<T> &other ) { return copy( other ); }
	Factory<T> &operator=( Factory<T> &&other ) { return move( static_cast<Factory<T> &&>( other ) ); }

	inline void init( const usize reserve = 1 )
	{
		// Sanity
		Assert( reserve > 0 && reserve != USIZE_MAX );
		Assert( data == nullptr );

		// Set State
		capacity = reserve;
		current = 0;

		// Allocate heap
		data = reinterpret_cast<T *>( memory_alloc( capacity * elementSizeBytes ) );
		Assert( data != nullptr );
		memory_set( data, 0, capacity * elementSizeBytes );

		puts( "Init!" );
	}

	inline void free()
	{
		// Check initialization
		if( data == nullptr ) { return; }

		// Call element destructors
		for( int i = 0; i < capacity; i++ )
		{
			const usize age = *reinterpret_cast<usize *>( reinterpret_cast<byte *>( data + i ) + sizeof( T ) );
			if( age != 0 ) { ( data + i )->~T(); }
		}

		// Free memory
		memory_free( data );
		data = nullptr;

		// Reset state
		capacity = 0;
		current = 0;

		puts( "Free!" );
	}

private:
	Factory<T> &copy( const Factory<T> &other )
	{
		// Prevent self-copying
		if( this == &other ) { return *this; }

		// Free current memory if necessary
		if( data != nullptr ) { free(); }

		// Copy other
		if( other.data != nullptr )
		{
			init( other.capacity );
			current = other.current;
			memory_copy( data, other.data, capacity * sizeof( elementSizeBytes ) );
		}
		else
		{
			data = nullptr;
			capacity = 0;
			current = 0;
		}

		// Return reference
		return *this;
	}

	Factory<T> &move( Factory<T> &&other )
	{
		// Prevent self-moving
		if( this == &other ) { return *this; }

		// Free current memory if necessary
		if( data != nullptr ) { free(); }

		// Copy other state
		data = other.data;
		capacity = other.capacity;
		current = other.current;

		// Reset other
		other.data = nullptr;
		other.capacity = 0;
		other.current = 0;

		// Return reference
		return *this;
	}

	inline void grow()
	{
		Assert( data != nullptr );
		Assert( capacity > 0 );
		capacity = capacity >= USIZE_MAX / 2 ? USIZE_MAX : capacity * 2;

		data = reinterpret_cast<T *>( memory_realloc( data, capacity * elementSizeBytes ) );
		Assert( data != nullptr );
	}

private:
	T     *data = nullptr;
	usize capacity = 0;
	usize current = 0;
	static constexpr usize elementSizeBytes = sizeof( T ) + sizeof( usize );
};


template <typename T>
class Factory<T, true> : public Factory<T, false>
{
public:
	Factory( const usize reserve = 1 ) { init( reserve ); }
	~Factory() { free(); }

private:
    using Factory<T>::init;
    using Factory<T>::free;
};