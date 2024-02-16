#pragma once

#include <config.hpp>

#include <vendor/stdarg.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_ONLY_DEBUG
	// Note: HEADER_ONLY_DEBUG used by boot.exe
	#define HEADER_ONLY_DEBUG ( 0 )
#endif

#ifndef COMPILE_DEBUG
	#define COMPILE_DEBUG ( 0 )
#endif

#ifndef COMPILE_DEBUG_SYMBOLS
	#define COMPILE_DEBUG_SYMBOLS ( 0 )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace debug
{
	extern void manta_vprintf( const bool newline, const char *format, va_list args );
	extern void manta_vprintf_color( const bool newline, unsigned char color, const char *format, va_list args );

	extern void manta_printf( const bool newline, const char *format, ... );
	extern void manta_printf_color( const bool newline, unsigned char color, const char *format, ... );

	extern void exit( int code );

	extern void manta_assert( const char *file, const char *function, const int line, const char *condition );
	extern void manta_assert_message( const char *file, const char *function, const int line, const char *condition, const char *msg, ... );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum
{
	LOG_DEFAULT = 0,
	LOG_BLACK = 30,
	LOG_RED = 31,
	LOG_GREEN = 32,
	LOG_YELLOW = 33,
	LOG_BLUE = 34,
	LOG_MAGENTA = 35,
	LOG_CYAN = 36,
	LOG_WHITE = 37,
};

#define Print( message, ... ) debug::manta_printf( false, message, ##__VA_ARGS__ )
#define PrintColor( color, message, ... ) debug::manta_printf_color( false, color, message, ##__VA_ARGS__ )
#define PrintLn( message, ... ) debug::manta_printf( true, message, ##__VA_ARGS__ )
#define PrintLnColor( color, message, ... ) debug::manta_printf_color( true, color, message, ##__VA_ARGS__ )

#if COMPILE_DEBUG
	#define DebugPrint( message, ... ) debug::manta_printf( false, message, ##__VA_ARGS__ )
	#define DebugPrintColor( color, message, ... ) debug::manta_printf_color( false, color, message, ##__VA_ARGS__ )
	#define DebugPrintLn( message, ... ) debug::manta_printf( true, message, ##__VA_ARGS__ )
	#define DebugPrintLnColor( color, message, ... ) debug::manta_printf_color( true, color, message, ##__VA_ARGS__ )
#else
	#define DebugPrint( message, ... )
	#define DebugPrintColor( color, message, ... )
	#define DebugPrintLn( message, ... )
	#define DebugPrintLnColor( color, message, ... )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace error
{
	extern int code;
}

extern void Error( const char *format, ... );
extern void ErrorIf( const bool condition, const char *format, ... );

#define ErrorReturn( value ) return value

#if COMPILE_DEBUG
	#define ErrorReturnIf( condition, value, message, ... ) \
		if( condition ) { debug::manta_printf_color( true, LOG_RED, "ERROR: " message " (%s)", ##__VA_ARGS__, __FILE__ ); return value; }

	#define ErrorReturnMsg( value, message, ... ) \
		debug::manta_printf_color( true, LOG_RED, "ERROR: " message " (%s)", ##__VA_ARGS__, __FILE__ ); return value
#else
	#define ErrorReturnIf( condition, value, message, ... ) \
		if( condition ) { debug::manta_printf_color( true, LOG_RED, "ERROR: " message, ##__VA_ARGS__ ); return value; }

	#define ErrorReturnMsg( value, message, ... ) \
		debug::manta_printf_color( true, LOG_RED, "ERROR: " message, ##__VA_ARGS__ ); return value
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if COMPILE_DEBUG
	#define Assert( condition ) \
		if( !( condition ) ) { debug::manta_assert( __FILE__, __FUNCTION__, __LINE__, #condition ); }

	#define AssertMsg( condition, message, ... ) \
		if( !( condition ) ) { debug::manta_assert_message( __FILE__, __FUNCTION__, __LINE__, #condition, message, ##__VA_ARGS__ ); }
#else
	#define Assert( condition )
	#define AssertMsg( condition, message, ... )
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if HEADER_ONLY_DEBUG
	// It's a bit gross to include a .cpp here, but build/project compile times
	// benefit from separating implementations from debug.hpp
	#include <debug.cpp>
#endif