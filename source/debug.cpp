#include <debug.hpp>

#include <vendor/stdio.hpp>
#include <vendor/stdlib.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Debug::manta_vprintf( const bool newline, const char *format, va_list args )
{
	vprintf( format, args );
	if( newline ) { printf( "\n" ); }
}


void Debug::manta_vprintf_color( const bool newline, unsigned char color, const char *format, va_list args )
{
	printf( "\x1b[%dm", color );
	vprintf( format, args );
	printf( newline ? "\x1b[%dm\n" : "\x1b[%dm", LOG_DEFAULT );
}


void Debug::manta_printf( const bool newline, const char *format, ... )
{
	va_list args;
	va_start( args, format );
	vprintf( format, args );
	if( newline ) { printf( "\n" ); }
	va_end( args );
}


void Debug::manta_printf_color( const bool newline, unsigned char color, const char *format, ... )
{
	va_list args;
	va_start( args, format );
	printf( "\x1b[%dm", color );
	vprintf( format, args );
	printf( newline ? "\x1b[%dm\n" : "\x1b[%dm", LOG_DEFAULT );
	va_end( args );
}


void Debug::exit( int code )
{
	iEngine::exitCode = code;
	::exit( code );
}


void Debug::manta_assert( const char *file, const char *function, const int line, const char *condition )
{
	PrintColor( LOG_RED, "\n\nASSERTION:\n\n" );
	PrintColor( LOG_RED, "> Failed: \"%s\"\n\n", condition );
	PrintColor( LOG_RED, "> Called: %s:%d\n", file, line );
	PrintColor( LOG_RED, "          %s\n\n", function );
	PrintColor( LOG_RED, "\n" );

	Debug::exit( 1 );
}



void Debug::manta_assert_message( const char *file, const char *function, const int line, const char *condition, const char *msg, ... )
{
	PrintColor( LOG_RED, "\n\nASSERTION:\n\n" );
	PrintColor( LOG_RED, "> Failed: \"%s\"\n\n", condition );
	PrintColor( LOG_RED, "> Called: %s:%d\n", file, line );
	PrintColor( LOG_RED, "          %s\n\n", function );

	va_list args;
	va_start( args, msg );
	manta_vprintf_color( true, LOG_RED, msg, args );
	va_end( args );
	PrintColor( LOG_RED, "\n" );

	Debug::exit( 1 );
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace iEngine
{
	int exitCode = 0;
	bool memoryLeakDetection = true;
}

void Error( const char *format, ... )
{
	PrintColor( LOG_RED, "\n\nERROR:\n\n" );

	va_list args;
	va_start( args, format );
	Debug::manta_vprintf_color( true, LOG_RED, format, args );
	va_end( args );

	PrintColor( LOG_RED, "\n" );

	Debug::exit( 1 );
}


void ErrorIf( const bool condition, const char *format, ... )
{
	if( !condition ) { return; }

	PrintColor( LOG_RED, "\n\nERROR:\n\n" );

	va_list args;
	va_start( args, format );
	Debug::manta_vprintf_color( true, LOG_RED, format, args );
	va_end( args );

	PrintColor( LOG_RED, "\n" );

	Debug::exit( 1 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
