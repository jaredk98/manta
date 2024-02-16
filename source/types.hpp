#pragma once

#include <pipeline.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using i8 = signed char;
#define I8_MIN ( -128 )
#define I8_MAX ( 127 )

using i16 = signed short;
#define I16_MIN ( -32768 )
#define I16_MAX ( 32768 )

using i32 = signed int;
#define I32_MIN ( -2147483648 )
#define I32_MAX ( 2147483647 )

using i64 = signed long long;
#define I64_MIN ( -9223372036854775808LL )
#define I64_MAX ( 9223372036854775807LL )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using u8 = unsigned char;
#define U8_MIN ( 0 )
#define U8_MAX ( 0xFF )

using u16 = unsigned short;
#define U16_MIN ( 0 )
#define U16_MAX ( 0xFFFF )

using u32 = unsigned int;
#define U32_MIN ( 0 )
#define U32_MAX ( 0xFFFFFFFF )

using u64 = unsigned long long;
#define U64_MIN ( 0 )
#define U64_MAX ( 0xFFFFFFFFFFFFFFFFUL )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if PIPELINE_ARCHITECTURE_X64 || PIPELINE_ARCHITECTURE_ARM64
	using isize = signed long long;
	#define ISIZE_MIN ( I64_MIN )
	#define ISIZE_MAX ( I64_MAX )

	using usize = unsigned long long;
	#define USIZE_MIN ( U64_MIN )
	#define USIZE_MAX ( U64_MAX )
#else
	static_assert( false, "Unsupported architecture!" );
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using r32 = float;
using r64 = double;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using Delta = double;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef byte
using byte = unsigned char;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define enum_type( name, type ) using name = type; enum : type

#define enum_namespace( name, type, ... ) \
	namespace name                        \
	{                                     \
		enum : type                       \
		{			                      \
			__VA_ARGS__                   \
		};			                      \
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ARRAY_LENGTH( x ) ( sizeof( x ) / sizeof( x[0] ) )

#define POW2_BITS( bits ) ( static_cast<u64>( 1 ) << ( bits ) )

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined( COMPILE_DEBUG ) && COMPILE_DEBUG
	static_assert( sizeof( i8 ) == 1,  "i8 not 1 byte!" );
	static_assert( sizeof( i16 ) == 2, "i16 not 2 bytes!" );
	static_assert( sizeof( i32 ) == 4, "i32 not 4 bytes!" );
	static_assert( sizeof( i64 ) == 8, "i64 not 8 bytes!" );

	static_assert( sizeof( u8 ) == 1,  "u8 not 1 byte!" );
	static_assert( sizeof( u16 ) == 2, "u16 not 2 bytes!" );
	static_assert( sizeof( u32 ) == 4, "u32 not 4 bytes!" );
	static_assert( sizeof( u64 ) == 8, "u64 not 8 bytes!" );

	#if PIPELINE_ARCHITECTURE_X64 || PIPELINE_ARCHITECTURE_ARM64
		static_assert( sizeof( isize ) == 8, "isize not 8 bytes! (x64/arm64)" );
		static_assert( sizeof( usize ) == 8, "usize not 8 bytes! (x64/arm64)" );
	#else
		static_assert( false, "Unsupported compiler architecture!" );
	#endif

	static_assert( sizeof( r32 ) == 4, "r32 not 4 bytes!" );
	static_assert( sizeof( r64 ) == 8, "r64 not 8 bytes!" );

	static_assert( sizeof( byte ) == 1, "byte not 1 byte!" );
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////