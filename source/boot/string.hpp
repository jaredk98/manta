#pragma once

#include <debug.hpp>
#include <types.hpp>

#include <vendor/stdarg.hpp>
#include <vendor/string.hpp>

#include <boot/platform.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace istring
{
	static void _strjoin( const usize bufferSize, char *buffer, ... )
	{
		if( buffer == nullptr || bufferSize == 0 ) { return; }

		va_list args;
		va_start( args, buffer );
		usize length = 0;

		const char *input;
		while( ( input = va_arg( args, const char * ) ) != nullptr )
		{
			usize inputLength = strlen( input );
			if( length + inputLength >= bufferSize )
			{
				buffer[length] = '\0';
				AssertMsg( false, "strjoin - length exceeded buffer size" )
				va_end( args );
				return;
			}
			strcpy( buffer + length, input );
			length += inputLength;
		}

		va_end( args );
		buffer[length] = '\0';
	}


	static void _filepath( const usize bufferSize, char *buffer, ... )
	{
		if( buffer == nullptr || bufferSize == 0 ) { return; }
		const usize slashLength = strlen( SLASH );

		va_list args;
		va_start( args, buffer );

		// Copy the first string
		buffer[0] = '\0';
		strncat( buffer, va_arg( args, const char * ), bufferSize - 1 );
		buffer[bufferSize - 1] = '\0';
		usize length = strlen( buffer );

		const char *input;
		while( ( input = va_arg( args, const char * ) ) != nullptr )
		{
			usize inputLength = slashLength + strlen( input );
			if( length + inputLength >= bufferSize )
			{
				buffer[length] = '\0';
				AssertMsg( false, "strjoin_filepath - length exceeded buffer size" )
				va_end( args );
				return;
			}
			strcpy( buffer + length, SLASH );
			strcpy( buffer + length + slashLength, input );
			length += inputLength;
		}

		va_end( args );
		buffer[length] = '\0';
	}


	static void _strappend( const usize bufferSize, char *buffer, const char *string )
	{
		if( buffer == nullptr || string == nullptr || bufferSize == 0 ) { return; }

		usize bufferLength = strlen( buffer );
		usize stringLength = strlen( string );

		AssertMsg( bufferLength + stringLength < bufferSize, "strappend - length exceeded buffer size" )
		if( bufferLength + stringLength < bufferSize ) { strncat( buffer, string, bufferSize - bufferLength - 1 ); }

		buffer[bufferSize - 1] = '\0';
	}
}

#define strjoin( buffer, ... ) istring::_strjoin( sizeof( buffer ), buffer, __VA_ARGS__, nullptr )
#define strjoin_filepath( buffer, ... ) istring::_filepath( sizeof( buffer ), buffer, __VA_ARGS__, nullptr );
#define strappend( buffer, string ) istring::_strappend( sizeof( buffer ), buffer, string )