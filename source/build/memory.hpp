#pragma once

#include <types.hpp>


extern void *memory_alloc( const usize size );

extern void *memory_realloc( void *block, const usize size );

extern void memory_copy( void *dst, const void *src, const usize size );

extern void memory_move( void *dst, const void *src, const usize size );

extern void memory_set( void *block, int c, const usize length );

extern void memory_free( void *block );

extern usize align_pow2( usize n );