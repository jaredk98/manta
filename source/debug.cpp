#include <debug.hpp>

#include <vendor/stdio.hpp>
#include <vendor/stdlib.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void debug::manta_vprintf( const bool newline, const char *format, va_list args )
{
	vprintf( format, args );
	if( newline ) { printf( "\n" ); }
}


void debug::manta_vprintf_color( const bool newline, unsigned char color, const char *format, va_list args )
{
	printf( "\x1b[%dm", color );
	vprintf( format, args );
	printf( newline ? "\x1b[%dm\n" : "\x1b[%dm", LOG_DEFAULT );
}


void debug::manta_printf( const bool newline, const char *format, ... )
{
	va_list args;
	va_start( args, format );
	vprintf( format, args );
	if( newline ) { printf( "\n" ); }
	va_end( args );
}


void debug::manta_printf_color( const bool newline, unsigned char color, const char *format, ... )
{
	va_list args;
	va_start( args, format );
	printf( "\x1b[%dm", color );
	vprintf( format, args );
	printf( newline ? "\x1b[%dm\n" : "\x1b[%dm", LOG_DEFAULT );
	va_end( args );
}


void debug::exit( int code )
{
	error::code = code;
	::exit( code );
}


void debug::manta_assert( const char *file, const char *function, const int line, const char *condition )
{
	PrintLnColor( LOG_RED, "\n\n====================================================================================================" );
	PrintColor( LOG_RED, "Assert:\n\tAssertion failed at %s, Function: %s (line: %d)\n\tCondition: %s\n", file, function, line, condition );

	debug::exit( 1 );
}



void debug::manta_assert_message( const char *file, const char *function, const int line, const char *condition, const char *msg, ... )
{
	PrintLnColor( LOG_RED, "\n\n====================================================================================================" );
	PrintColor( LOG_RED, "Assert:\n\tAssertion failed at %s, Function: %s (line: %d)\n\tCondition: %s\nMESSAGE:\n\t", file, function, line, condition );
	va_list args;
	va_start( args, msg );
	manta_vprintf_color( true, LOG_RED, msg, args );
	va_end( args );

	debug::exit( 1 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace error
{
	int code = 0;
}


void Error( const char *format, ... )
{
	PrintLnColor( LOG_RED, "\n\n====================================================================================================" );
	PrintColor( LOG_RED, "ERROR: " );

	va_list args;
	va_start( args, format );
	debug::manta_vprintf_color( true, LOG_RED, format, args );
	va_end( args );

	debug::exit( 1 );
}


void ErrorIf( const bool condition, const char *format, ... )
{
	if( !condition ) { return; }
	PrintLnColor( LOG_RED, "\n\n====================================================================================================" );
	PrintColor( LOG_RED, "ERROR: " );

	va_list args;
	va_start( args, format );
	debug::manta_vprintf_color( true, LOG_RED, format, args );
	va_end( args );

	debug::exit( 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
