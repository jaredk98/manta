#include <manta/fileio.hpp>

#include <manta/memory.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char WORKING_DIRECTORY[PATH_SIZE];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void path_change_extension( char *buffer, const usize bufferSize, const char *path, const char *extension )
{
	// Invalid input parameters?
	if( buffer == nullptr || path == nullptr || extension == nullptr || bufferSize == 0 )
	{
		AssertMsg( false, "path_change_extension - invalid parameters (buffer: %s (%llu), path: %s, extension: %s)",
		               buffer, bufferSize, path, extension );
		buffer[0] = '\0';
		return;
	}

	// Check for sufficient buffer size
	usize pathLength = strlen( path );
	usize extensionLength = strlen( extension );
	if( pathLength >= bufferSize || extensionLength >= bufferSize )
	{
		AssertMsg( false, "path_change_extension - insufficient buffer size for changing extension (buffer: %s (%llu), path: %s, extension: %s)",
		               buffer, bufferSize, path, extension );
		buffer[0] = '\0';
		return;
	}

	// Copy Base
	strncpy( buffer, path, bufferSize - 1 );
	buffer[bufferSize - 1] = '\0';

	// Find Last '.'
	char *lastDot = strrchr( buffer, '.' );
	if( lastDot != nullptr )
	{
		// Change Extension
		AssertMsg( static_cast<int>( lastDot - buffer ) + extensionLength < bufferSize,
					   "path_change_extension - insufficient buffer size for changing extension (buffer: %s (%llu), path: %s, extension: %s)",
					   buffer, bufferSize, path, extension );
		strncpy( lastDot, extension, bufferSize - ( lastDot - buffer ) - 1 );
	}
	else
	{
		// No dot found, append the extension
		AssertMsg( strlen( buffer ) + extensionLength < bufferSize,
		               "path_change_extension - insufficient buffer size for appending extension (buffer: %s (%llu), path: %s, extension: %s)",
		               buffer, bufferSize, path, extension );
		strncat( buffer, extension, bufferSize - strlen( buffer ) - 1 );
	}

	// Success
	buffer[bufferSize - 1] = '\0';
}


void path_get_directory( char *buffer, const usize bufferSize, const char *path )
{
	char *result = buffer;
	char *base = buffer;

	// Copy Base
	strncpy( buffer, path, bufferSize - 1 );
	buffer[bufferSize - 1] = '\0';

	// Find Last SLASH
	while( *buffer )
	{
		if( *buffer++ == SLASH_CHAR ) { base = buffer; }
	}

	*( base - 1 ) = '\0';
}


void swrite( const char *string, FILE *file )
{
	fwrite( string, strlen( string ), 1, file );
}


usize fsize( FILE *file )
{
	fseek( file, 0, SEEK_END );
	usize size = ftell( file );
	fseek( file, 0, SEEK_SET );
	return size;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool File::open( const char *path )
{
	// Close current file if one is open
	if( data != nullptr ) { close(); }
	Assert( file == nullptr );

	// Try to open file for reading
	filepath = path;
	file = fopen( filepath, "rb" );
	if( file == nullptr ) { return false; }

	// Get file size
	size = fsize( file );
	if( size == 0 ) { goto cleanup; }

	// Allocate memory memory & read file contents into 'data'
	data = reinterpret_cast<byte *>( memory_alloc( size + 1 ) );
	data[size] = '\0';
	if( fread( data, size, 1, file ) < 1 ) { goto cleanup; }

	// Success
	return true;

cleanup:
	fclose( file );
	if( data != nullptr ) { memory_free( data ); }
	file = nullptr;
	data = nullptr;
	size = 0;
	return false;
}


bool File::save( const char *path )
{
	// Skip if there is no file open
	if( data == nullptr ) { return false; }
	if( file == nullptr ) { return false; }

	// Open file for writing
	FILE *wfile = fopen( path, "wb" );
	if( wfile == nullptr ) { return false; }

	// Write file
	if( fwrite( data, size, 1, wfile ) < 1 ) { goto cleanup; }

	// Success
	return true;

cleanup:
	fclose( wfile );
	return false;
}


bool File::close()
{
	// Free memory
	if( data != nullptr ) { memory_free( data ); }
	data = nullptr;
	size = 0;

	// Close file
	if( file == nullptr ) { return true; }
	if( fclose( file ) != 0 ) { return false; }
	file = nullptr;
	return true;
}