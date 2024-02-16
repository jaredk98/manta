#include <build/fileio.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if OS_WINDOWS

	bool file_time( const char *path, FileTime *result )
	{
		// Open File
		HANDLE file;
		if( ( file = CreateFileA( path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr ) ) == INVALID_HANDLE_VALUE )
		{
			return false;
		}

		// Get File Time
		bool success = true;
		if( !GetFileTime( file, nullptr, nullptr, &result->time ) ) { success = false; }

		// Close File
		CloseHandle( file );

		// Success?
		return success;
	}


	bool file_time_newer( const FileTime &a, const FileTime &b )
	{
		ULARGE_INTEGER integerA;
		integerA.LowPart  = a.time.dwLowDateTime;
		integerA.HighPart = a.time.dwHighDateTime;

		ULARGE_INTEGER integerB;
		integerB.LowPart  = b.time.dwLowDateTime;
		integerB.HighPart = b.time.dwHighDateTime;

		return integerA.QuadPart > integerB.QuadPart;
	}


	void directory_create( const char *path )
	{
		CreateDirectoryA( path, nullptr );
	}


	bool directory_iterate( List<FileInfo> &list, const char *path, const char *extension, const bool recurse )
	{
		WIN32_FIND_DATAA findData;
		HANDLE findFile;

		// Find First File
		char buffer[512];
		strjoin( buffer, path, SLASH, "*" );
		if( ( findFile = FindFirstFileA( buffer, &findData ) ) == INVALID_HANDLE_VALUE ) { return false; }

		do
		{
			// Ignore Hidden Directories
			if( findData.cFileName[0] == '.' ) { continue; }

			// Recurse Into Directories
			if( findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && recurse )
			{
				strjoin( buffer, path, SLASH, findData.cFileName );
				directory_iterate( list, buffer, extension, recurse );
			}
			// Add File
			else
			{
				// Filter extension
				const int length = static_cast<int>( strlen( findData.cFileName ) );
				const int extensionLength = static_cast<int>( strlen( extension ) );
				if( length <= extensionLength || strcmp( findData.cFileName + length - extensionLength, extension ) ) { continue; }

				// Add FileInfo
				FileInfo info;
				strjoin( info.path, path, SLASH, findData.cFileName );
				strncpy( info.name, findData.cFileName, sizeof( info.path ) - 1 );
				info.name[sizeof( info.path ) - 1] = '\0';
				info.time.time = findData.ftLastWriteTime;
				list.add( info );
			}
		} while ( FindNextFileA( findFile, &findData ) );

		// Success
		return true;
	}

#else

	bool file_time( const char *path, FileTime *result )
	{
		struct stat file_stat;
		int file = open( path, O_RDONLY);
		if( file == -1 ) { return false; }
		if( fstat( file, &file_stat ) == -1 ) { return false; }

		#if PIPELINE_OS_MACOS
		result->time = static_cast<u64>( file_stat.st_mtime );
		#else
		result->time = static_cast<u64>( file_stat.st_mtim.tv_sec ) * 1000000 + static_cast<u64>( file_stat.st_mtim.tv_nsec ) / 1000;
		#endif

		return true;
	}


	bool file_time_newer( const FileTime &a, const FileTime &b )
	{
		return a.time > b.time;
	}


	void directory_create( const char *path )
	{
		char dir[PATH_SIZE];
		strjoin( dir, "." SLASH, path );
		mkdir( dir, 0777 );
	}


	bool directory_iterate( List<FileInfo> &list, const char *path, const char *extension, const bool recurse )
	{
		struct dirent *entry;
		DIR *dir = opendir( path );
		if( !dir ) { return false; }

		// Find first file
		if( ( entry = readdir( dir ) ) == nullptr ) { closedir( dir ); return false; }

		do
		{
			// Ignore hidden directories
			if( entry->d_name[0] == '.' ) { continue; }

			// Recurse Into Directories
			if( entry->d_type == DT_DIR && recurse )
			{
				char buffer[512];
				strjoin( buffer, path, SLASH, entry->d_name );
				directory_iterate( list, buffer, extension, recurse );
			}
			// Add File
			else
			{
				// Filter extension
				const int length = strlen( entry->d_name );
				const int extensionLength = strlen( extension );
				if( length <= extensionLength || strcmp( entry->d_name + length - extensionLength, extension ) != 0 ) { continue; }

				// Add FileInfo
				FileInfo info;
				strjoin( info.path, path, SLASH, entry->d_name );
				strncpy( info.name, entry->d_name, sizeof( info.name ) - 1 );
				info.name[sizeof( info.name ) - 1] = '\0';
				file_time( path, &info.time );
				list.add( info );
			}
		} while ( ( entry = readdir( dir ) ) != nullptr );

		closedir( dir );
		return true;
	}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void path_change_extension( char *buffer, usize bufferSize, const char *path, const char *extension )
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

	// Null terminate at last slash
	*( base - 1 ) = '\0';
}


void path_get_filename( char *buffer, const usize bufferSize, const char *path )
{
	char c;
	const char *slash = path;

	// Find the last slash
	while( ( c = *path++ ) != '\0' )
	{
		if( c == SLASH_CHAR ) { slash = path; }
	}

	// Copy filename to buffer
	const usize size = strlen( slash );
	if( size >= bufferSize ) { buffer[0] = '\0'; return; }
	strncpy( buffer, slash, size );
	buffer[size] = '\0';
}


void path_remove_extension( char *path )
{
	char c;
	char *extension = path;

	// Find the last '.'
	while( ( c = *++path ) != '\0' )
	{
		if( c == '.' ) { extension = path; }
	}

	*extension = '\0';
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool file_copy( const char *srcPath, const char *dstPath )
{
	// Open file
	File file { srcPath };
	if( !file )
	{
		file.close();
		return false;
	}

	// Save file
	if( !file.save( dstPath ) )
	{
		file.close();
		return false;
	}

	// Success
	file.close();
	return true;
}