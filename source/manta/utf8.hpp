#pragma once

#include <types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UTF8_ACCEPT 0

// Maximum of 4 bytes per utf8 char, UTF8_BYTES + 1 for the null termiator
#define UTF8_BYTES ( 4 )
#define UTF8_BYTES_MAX ( UTF8_BYTES + 1 )

// 0xC0 = 11000000
// 0x80 = 10000000
#define UTF8_IS_CONTINUATION_BYTE( x ) ( ( ( x ) & 0xC0 ) == 0x80 )

extern int utf8_string_length_chars( const char *string );
extern int utf8_string_length_bytes( const char *string );
extern int utf8_substr_length_bytes( const char *string, const int charStart, const int charEnd );

extern int utf8_encode( char *buffer, u32 code );
extern u32 utf8_decode( u32 *state, u32 *code, char byte );