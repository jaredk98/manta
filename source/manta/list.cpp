#include <manta/list.hpp>
#include <manta/memory.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ilist
{
	void List::init( const usize reserve )
	{
		Assert( data == nullptr );
		capacity = reserve;
		current = 0;
		data = reinterpret_cast<byte *>( memory_alloc( capacity * stride ) );
		ErrorIf( data == nullptr, "Failed to allocate memory for init() List (%p: memory_alloc %d bytes)", data, capacity * stride );
	}


	void List::free()
	{
		memory_free( data );
		data = nullptr;
	}


	void List::grow()
	{
		Assert( data != nullptr );
		ErrorIf( capacity == USIZE_MAX, "Failed to grow() List -- it is already at capacity (%p: capacity: %d bytes)", data, capacity * stride );
		capacity = capacity == 0 ? 1 : ( capacity >= USIZE_MAX / 2 ? USIZE_MAX : capacity * 2 );
		data = reinterpret_cast<byte *>( memory_realloc( data, capacity * stride ) );
		ErrorIf( data == nullptr, "Failed to reallocate memory for grow() List (%p: memory_realloc %d bytes)", data, capacity * stride );
	}


	void List::copy( const List &other )
	{
		Assert( other.data != nullptr );
		if( data != nullptr ) { free(); }
		capacity = other.capacity;
		current = other.current;
		stride = other.stride;
		data = reinterpret_cast<byte *>( memory_alloc( capacity * stride ) );
		ErrorIf( data == nullptr, "Failed to allocate memory for copy() List (%p: memory_alloc %d bytes)", data, capacity * stride );
		memory_copy( data, other.data, capacity * stride );
	}


	void List::move( List &&other )
	{
		if( data != nullptr ) { free(); }
		data = other.data;
		capacity = other.capacity;
		current = other.current;
		stride = other.stride;

		other.data = nullptr;
		other.capacity = 0;
		other.current = 0;
	}


	byte *List::add_at()
	{
		Assert( data != nullptr );
		Assert( current < U64_MAX );
		if( current == capacity ) { grow(); }
		return data + ( current++ ) * stride;
	}


	byte *List::insert_at( const usize index )
	{
		Assert( data != nullptr );
		Assert( index <= current );
		if( current == capacity ) { grow(); }
		const usize size = ( current++ ) - index;
		if( size != 0 ) { memory_move( data + ( index + 1 ) * stride, data + index * stride, size * stride ); }
		return data + index * stride;
	}


	byte *List::remove_at( const usize index )
	{
		Assert( data != nullptr );
		Assert( index < current );
		const usize size = ( --current ) - index;
		if( size > 0 ) { memory_move( data + index * stride, data + ( index + 1 ) * stride, size * stride ); }
		return data + index * stride;
	}


	byte *List::remove_swap_at( const usize index )
	{
		Assert( data != nullptr );
		Assert( index < current );
		if( --current == index ) { return data + index * stride; }
		memory_copy( data + index * stride, data + current * stride, stride );
		return data + index * stride;
	}
}