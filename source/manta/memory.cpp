#include <manta/memory.hpp>

#include <vendor/stdlib.hpp>
#include <vendor/string.hpp>

#include <debug.hpp>
#include <pipeline.hpp>


void *memory_alloc( const usize size )
{
	return malloc( size );
}


void *memory_realloc( void *block, const usize size )
{
	Assert( block != nullptr );
	return realloc( block, size );
}


void memory_copy( void *dst, const void *src, const usize size )
{
	Assert( dst != nullptr );
	Assert( src != nullptr );
	memcpy( dst, src, size );
}


void memory_move( void *dst, const void *src, const usize size )
{
	Assert( dst != nullptr );
	Assert( src != nullptr );
	memmove( dst, src, size );
}


void memory_set( void *block, int c, const usize length )
{
	Assert( block != nullptr );
	memset( block, c, length );
}


void memory_free( void *block )
{
	Assert( block != nullptr );
	free( block );
}


usize align_pow2( usize n )
{
	if( n == 0 ) { return 1; }

	n -= 1;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
#if PIPELINE_ARCHITECTURE_64BIT
	n |= n >> 32;
#endif
	return n + 1;
}
