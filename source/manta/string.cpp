#include <manta/string.hpp>

#include <manta/fileio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void iString::_strjoin( const usize bufferSize, char *buffer, ... )
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


void iString::_strjoin_filepath( const usize bufferSize, char *buffer, ... )
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


void iString::_strappend( const usize bufferSize, char *buffer, const char *string )
{
	if( buffer == nullptr || string == nullptr || bufferSize == 0 ) { return; }

	usize bufferLength = strlen( buffer );
	usize stringLength = strlen( string );

	AssertMsg( bufferLength + stringLength < bufferSize, "strappend - length exceeded buffer size" )
	if( bufferLength + stringLength < bufferSize ) { strncat( buffer, string, bufferSize - bufferLength - 1 ); }

	buffer[bufferSize - 1] = '\0';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool String::save( const char *path )
{
	Assert( data != nullptr );
	FILE *file = fopen( path, "wb" );
	if( file == nullptr ) { return false; }

	bool returnCode = true;
	if( fwrite( data, current, 1, file ) < 1 ) { returnCode = false; goto cleanup; }

cleanup:
	if( fclose( file ) != 0 ) { return false; }
	return returnCode;
}


bool String::load( const char *path )
{
	Assert( data != nullptr );
	FILE *file = fopen( path, "rb" );
	if( file == nullptr ) { return false; }

	bool returnCode = true;
	const usize size = fsize( file );
	if( size == 0 ) { returnCode = false; goto cleanup; }

	capacity = size + 1;
	current = size;
	data = reinterpret_cast<char *>( memory_realloc( data, capacity ) );
	ErrorIf( data == nullptr, "Failed to allocate memory for load() String (%p: memory_alloc %d bytes)", data, capacity );

	if( fread( data, size, 1, file ) < 1 ) { returnCode = false; goto cleanup; }
	data[size] = '\0';

cleanup:
	if( fclose( file ) != 0 ) { return false; }
	return returnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////